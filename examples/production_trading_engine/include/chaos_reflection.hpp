#ifndef CHAOS_REFLECTION_HPP
#define CHAOS_REFLECTION_HPP

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <sstream>
#include <cstdint>
#include <cstring>
#include <cctype>
#include <type_traits>
#include <chaos/preprocessor.h>

namespace chaos_reflection {

// ============================================================================
// Lightweight Zero-Dependency JSON Parser and Node Model
// ============================================================================

struct JsonNode {
    enum Type { NIL, NUMBER, STRING, BOOLEAN, OBJECT, ARRAY } type = NIL;
    std::string string_val;
    double number_val = 0.0;
    uint64_t uint_val = 0;
    int64_t int_val = 0;
    bool is_integer = false;
    bool bool_val = false;
    std::vector<std::pair<std::string, JsonNode>> obj_val;
    std::vector<JsonNode> arr_val;

    const JsonNode* get(std::string_view key) const {
        for (const auto& kv : obj_val) {
            if (kv.first == key) return &kv.second;
        }
        return nullptr;
    }
};

class JsonParser {
    std::string_view src;
    size_t pos = 0;

    void skip_ws() {
        while (pos < src.size() && (src[pos] == ' ' || src[pos] == '\t' || src[pos] == '\n' || src[pos] == '\r')) {
            ++pos;
        }
    }

    char peek() { skip_ws(); return pos < src.size() ? src[pos] : '\0'; }
    char get() { skip_ws(); return pos < src.size() ? src[pos++] : '\0'; }

    std::string parse_string() {
        if (get() != '"') return {};
        std::string s;
        while (pos < src.size()) {
            char c = src[pos++];
            if (c == '"') return s;
            if (c == '\\' && pos < src.size()) {
                char esc = src[pos++];
                switch (esc) {
                    case 'n': s += '\n'; break;
                    case 't': s += '\t'; break;
                    case 'r': s += '\r'; break;
                    case '"': s += '"'; break;
                    case '\\': s += '\\'; break;
                    default: s += esc; break;
                }
            } else {
                s += c;
            }
        }
        return s;
    }

    JsonNode parse_number() {
        size_t start = pos;
        bool has_dot_or_exp = false;
        bool is_neg = false;
        if (pos < src.size() && (src[pos] == '-' || src[pos] == '+')) {
            if (src[pos] == '-') is_neg = true;
            ++pos;
        }
        while (pos < src.size() && (std::isdigit(src[pos]) || src[pos] == '.' || 
               src[pos] == 'e' || src[pos] == 'E' || src[pos] == '-' || src[pos] == '+')) {
            if (src[pos] == '.' || src[pos] == 'e' || src[pos] == 'E') {
                has_dot_or_exp = true;
            }
            ++pos;
        }
        std::string num_str(src.substr(start, pos - start));
        JsonNode node;
        node.type = JsonNode::NUMBER;
        if (!has_dot_or_exp) {
            node.is_integer = true;
            try {
                if (is_neg) {
                    node.int_val = std::stoll(num_str);
                    node.uint_val = static_cast<uint64_t>(node.int_val);
                    node.number_val = static_cast<double>(node.int_val);
                } else {
                    node.uint_val = std::stoull(num_str);
                    node.int_val = static_cast<int64_t>(node.uint_val);
                    node.number_val = static_cast<double>(node.uint_val);
                }
            } catch (...) {
                node.int_val = 0;
                node.uint_val = 0;
                node.number_val = 0.0;
            }
        } else {
            try {
                node.number_val = std::stod(num_str);
            } catch (...) {
                node.number_val = 0.0;
            }
        }
        return node;
    }

public:
    explicit JsonParser(std::string_view s) : src(s) {}

    JsonNode parse_val() {
        char c = peek();
        if (c == '"') {
            JsonNode n; n.type = JsonNode::STRING; n.string_val = parse_string(); return n;
        } else if (c == '{') {
            get();
            JsonNode n; n.type = JsonNode::OBJECT;
            while (peek() != '}' && peek() != '\0') {
                std::string key = parse_string();
                if (get() != ':') break;
                JsonNode child = parse_val();
                n.obj_val.emplace_back(std::move(key), std::move(child));
                if (peek() == ',') get();
            }
            if (peek() == '}') get();
            return n;
        } else if (c == '[') {
            get();
            JsonNode n; n.type = JsonNode::ARRAY;
            while (peek() != ']' && peek() != '\0') {
                n.arr_val.push_back(parse_val());
                if (peek() == ',') get();
            }
            if (peek() == ']') get();
            return n;
        } else if (c == 't' || c == 'f') {
            JsonNode n; n.type = JsonNode::BOOLEAN;
            if (src.substr(pos, 4) == "true") { pos += 4; n.bool_val = true; }
            else if (src.substr(pos, 5) == "false") { pos += 5; n.bool_val = false; }
            return n;
        } else if (c == 'n') {
            pos += 4;
            JsonNode n; n.type = JsonNode::NIL; return n;
        } else {
            return parse_number();
        }
    }
};

// ============================================================================
// Type Traits Detection
// ============================================================================

template<typename T, typename = void>
struct is_reflected : std::false_type {};

template<typename T>
struct is_reflected<T, std::void_t<typename T::is_chaos_reflected_struct>> : std::true_type {};

template<typename T>
inline constexpr bool is_reflected_v = is_reflected<T>::value;

template<typename T>
struct is_vector : std::false_type {};

template<typename T, typename A>
struct is_vector<std::vector<T, A>> : std::true_type {};

template<typename T>
inline constexpr bool is_vector_v = is_vector<T>::value;

// Forward declarations
template<typename T>
bool parse_json_value(const JsonNode& node, T& val);

template<typename T>
void serialize_json_value(std::ostream& os, const T& val, int indent);

inline void print_indent(std::ostream& os, int indent) {
    for (int i = 0; i < indent; ++i) os << "  ";
}

// ============================================================================
// JSON Serialization Implementation
// ============================================================================

template<typename T>
void serialize_json_value(std::ostream& os, const T& val, int indent) {
    if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
        os << '"';
        for (char c : val) {
            if (c == '"') os << "\\\"";
            else if (c == '\\') os << "\\\\";
            else if (c == '\n') os << "\\n";
            else if (c == '\r') os << "\\r";
            else if (c == '\t') os << "\\t";
            else os << c;
        }
        os << '"';
    } else if constexpr (std::is_same_v<T, bool>) {
        os << (val ? "true" : "false");
    } else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
        os << val;
    } else if constexpr (std::is_enum_v<T>) {
        os << '"' << to_string(val) << '"';
    } else if constexpr (is_vector_v<T>) {
        if (val.empty()) {
            os << "[]";
            return;
        }
        os << "[\n";
        for (size_t i = 0; i < val.size(); ++i) {
            if (i > 0) os << ",\n";
            print_indent(os, indent + 1);
            serialize_json_value(os, val[i], indent + 1);
        }
        os << "\n";
        print_indent(os, indent);
        os << "]";
    } else if constexpr (is_reflected_v<T>) {
        os << "{\n";
        bool first = true;
        for_each_field(val, [&](std::string_view name, const auto& f_val) {
            if (!first) os << ",\n";
            first = false;
            print_indent(os, indent + 1);
            os << '"' << name << "\": ";
            serialize_json_value(os, f_val, indent + 1);
        });
        os << "\n";
        print_indent(os, indent);
        os << "}";
    }
}

template<typename T>
std::string to_json(const T& val) {
    std::ostringstream oss;
    serialize_json_value(oss, val, 0);
    return oss.str();
}

// ============================================================================
// JSON Deserialization Implementation
// ============================================================================

template<typename T>
bool parse_json_value(const JsonNode& node, T& val) {
    if constexpr (std::is_same_v<T, std::string>) {
        if (node.type != JsonNode::STRING) return false;
        val = node.string_val;
        return true;
    } else if constexpr (std::is_same_v<T, bool>) {
        if (node.type == JsonNode::BOOLEAN) { val = node.bool_val; return true; }
        if (node.type == JsonNode::NUMBER) { val = (node.number_val != 0.0); return true; }
        return false;
    } else if constexpr (std::is_integral_v<T>) {
        if (node.type != JsonNode::NUMBER) return false;
        if (node.is_integer) {
            if constexpr (std::is_unsigned_v<T>) {
                val = static_cast<T>(node.uint_val);
            } else {
                val = static_cast<T>(node.int_val);
            }
        } else {
            val = static_cast<T>(node.number_val);
        }
        return true;
    } else if constexpr (std::is_floating_point_v<T>) {
        if (node.type != JsonNode::NUMBER) return false;
        val = static_cast<T>(node.number_val);
        return true;
    } else if constexpr (std::is_enum_v<T>) {
        if (node.type != JsonNode::STRING) return false;
        return parse_enum(node.string_val, val);
    } else if constexpr (is_vector_v<T>) {
        if (node.type != JsonNode::ARRAY) return false;
        val.clear();
        val.reserve(node.arr_val.size());
        for (const auto& item_node : node.arr_val) {
            typename T::value_type item{};
            if (!parse_json_value(item_node, item)) return false;
            val.push_back(std::move(item));
        }
        return true;
    } else if constexpr (is_reflected_v<T>) {
        if (node.type != JsonNode::OBJECT) return false;
        for (const auto& [k, child_node] : node.obj_val) {
            deserialize_field_by_key(val, k, child_node);
        }
        return true;
    }
    return false;
}

template<typename T>
bool from_json(std::string_view json_str, T& out) {
    JsonParser parser(json_str);
    JsonNode root = parser.parse_val();
    return parse_json_value(root, out);
}

// ============================================================================
// High-Performance Binary Wire Protocol Serialization
// ============================================================================

template<typename T>
void write_binary(std::vector<uint8_t>& buf, const T& val) {
    if constexpr (std::is_same_v<T, std::string>) {
        uint32_t sz = static_cast<uint32_t>(val.size());
        const uint8_t* sz_p = reinterpret_cast<const uint8_t*>(&sz);
        buf.insert(buf.end(), sz_p, sz_p + sizeof(sz));
        const uint8_t* p = reinterpret_cast<const uint8_t*>(val.data());
        buf.insert(buf.end(), p, p + sz);
    } else if constexpr (std::is_enum_v<T>) {
        uint8_t b = static_cast<uint8_t>(val);
        buf.push_back(b);
    } else if constexpr (std::is_fundamental_v<T>) {
        const uint8_t* p = reinterpret_cast<const uint8_t*>(&val);
        buf.insert(buf.end(), p, p + sizeof(T));
    } else if constexpr (is_vector_v<T>) {
        uint32_t count = static_cast<uint32_t>(val.size());
        const uint8_t* count_p = reinterpret_cast<const uint8_t*>(&count);
        buf.insert(buf.end(), count_p, count_p + sizeof(count));
        for (const auto& item : val) {
            write_binary(buf, item);
        }
    } else if constexpr (is_reflected_v<T>) {
        for_each_field(val, [&](std::string_view, const auto& f) {
            write_binary(buf, f);
        });
    }
}

template<typename T>
bool read_binary(const uint8_t*& ptr, size_t& len, T& val) {
    if constexpr (std::is_same_v<T, std::string>) {
        if (len < sizeof(uint32_t)) return false;
        uint32_t sz = 0;
        std::memcpy(&sz, ptr, sizeof(sz));
        ptr += sizeof(sz);
        len -= sizeof(sz);
        if (len < sz) return false;
        val.assign(reinterpret_cast<const char*>(ptr), sz);
        ptr += sz;
        len -= sz;
        return true;
    } else if constexpr (std::is_enum_v<T>) {
        if (len < 1) return false;
        val = static_cast<T>(*ptr++);
        len -= 1;
        return true;
    } else if constexpr (std::is_fundamental_v<T>) {
        if (len < sizeof(T)) return false;
        std::memcpy(&val, ptr, sizeof(T));
        ptr += sizeof(T);
        len -= sizeof(T);
        return true;
    } else if constexpr (is_vector_v<T>) {
        if (len < sizeof(uint32_t)) return false;
        uint32_t count = 0;
        std::memcpy(&count, ptr, sizeof(count));
        ptr += sizeof(count);
        len -= sizeof(count);
        val.clear();
        val.reserve(count);
        for (uint32_t i = 0; i < count; ++i) {
            typename T::value_type item{};
            if (!read_binary(ptr, len, item)) return false;
            val.push_back(std::move(item));
        }
        return true;
    } else if constexpr (is_reflected_v<T>) {
        bool ok = true;
        for_each_field(val, [&](std::string_view, auto& f) {
            if (ok) ok = read_binary(ptr, len, f);
        });
        return ok;
    }
    return false;
}

} // namespace chaos_reflection

// ============================================================================
// Chaos Preprocessor Reflection Macro Definitions
// ============================================================================

#define CHAOS_ENUM_ITEM(_s, elem) elem,
#define CHAOS_ENUM_TO_STR(_s, elem, Name) if (val == Name::elem) return #elem;
#define CHAOS_ENUM_FROM_STR(_s, elem, Name) if (str == #elem) { out = Name::elem; return true; }

#define CHAOS_REFLECT_ENUM(Name, values) \
    enum class Name : uint8_t { \
        CHAOS_PP_EXPR(CHAOS_PP_SEQ_FOR_EACH(CHAOS_ENUM_ITEM, values)) \
    }; \
    inline const char* to_string(Name val) { \
        CHAOS_PP_EXPR(CHAOS_PP_SEQ_FOR_EACH(CHAOS_ENUM_TO_STR, values, Name)) \
        return "UNKNOWN"; \
    } \
    inline bool parse_enum(std::string_view str, Name& out) { \
        CHAOS_PP_EXPR(CHAOS_PP_SEQ_FOR_EACH(CHAOS_ENUM_FROM_STR, values, Name)) \
        return false; \
    } \
    inline std::ostream& operator<<(std::ostream& os, Name val) { \
        return os << to_string(val); \
    }

#define CHAOS_FIELD_DECL(_s, elem) \
    CHAOS_PP_TUPLE_ELEM(2, 0, elem) CHAOS_PP_TUPLE_ELEM(2, 1, elem){};

#define CHAOS_FIELD_NAME_STR(_s, elem) \
    CHAOS_PP_STRINGIZE(CHAOS_PP_TUPLE_ELEM(2, 1, elem)),

#define CHAOS_FIELD_VISIT(_s, elem) \
    visitor(CHAOS_PP_STRINGIZE(CHAOS_PP_TUPLE_ELEM(2, 1, elem)), obj.CHAOS_PP_TUPLE_ELEM(2, 1, elem));

#define CHAOS_FIELD_EQ(_s, elem) \
    && (this->CHAOS_PP_TUPLE_ELEM(2, 1, elem) == other.CHAOS_PP_TUPLE_ELEM(2, 1, elem))

#define CHAOS_FIELD_PARSE_ENTRY(_s, elem) \
    if (key == CHAOS_PP_STRINGIZE(CHAOS_PP_TUPLE_ELEM(2, 1, elem))) { \
        return ::chaos_reflection::parse_json_value(node, obj.CHAOS_PP_TUPLE_ELEM(2, 1, elem)); \
    }

#define CHAOS_REFLECT_STRUCT(TypeName, fields) \
    struct TypeName { \
        using is_chaos_reflected_struct = void; \
        CHAOS_PP_EXPR(CHAOS_PP_SEQ_FOR_EACH(CHAOS_FIELD_DECL, fields)) \
        static constexpr size_t field_count = CHAOS_PP_EXPR(CHAOS_PP_SEQ_SIZE(fields)); \
        static constexpr std::array<std::string_view, field_count> field_names = { \
            CHAOS_PP_EXPR(CHAOS_PP_SEQ_FOR_EACH(CHAOS_FIELD_NAME_STR, fields)) \
        }; \
        template<typename Visitor> \
        friend void for_each_field(const TypeName& obj, Visitor&& visitor) { \
            CHAOS_PP_EXPR(CHAOS_PP_SEQ_FOR_EACH(CHAOS_FIELD_VISIT, fields)) \
        } \
        template<typename Visitor> \
        friend void for_each_field(TypeName& obj, Visitor&& visitor) { \
            CHAOS_PP_EXPR(CHAOS_PP_SEQ_FOR_EACH(CHAOS_FIELD_VISIT, fields)) \
        } \
        bool operator==(const TypeName& other) const { \
            return true CHAOS_PP_EXPR(CHAOS_PP_SEQ_FOR_EACH(CHAOS_FIELD_EQ, fields)); \
        } \
        bool operator!=(const TypeName& other) const { \
            return !(*this == other); \
        } \
        friend bool deserialize_field_by_key(TypeName& obj, std::string_view key, const ::chaos_reflection::JsonNode& node) { \
            CHAOS_PP_EXPR(CHAOS_PP_SEQ_FOR_EACH(CHAOS_FIELD_PARSE_ENTRY, fields)) \
            return false; \
        } \
        friend std::ostream& operator<<(std::ostream& os, const TypeName& obj) { \
            return os << ::chaos_reflection::to_json(obj); \
        } \
    };

#endif // CHAOS_REFLECTION_HPP
