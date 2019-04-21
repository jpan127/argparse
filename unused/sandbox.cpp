
// template <typename InputType, typename OutputType, typename ... Types>
// OutputType type_switch(const InputType value, const std::function<OutputType(const Types &)> & ... cbs) {
//     for (auto && cb : {cbs...}) {
//         using CallbackType = typename decltype(cb)::argument_type;
//         if (std::is_same<InputType, CallbackType>::value) {
//             return cb(value);
//         }
//     }
// }

// template <Variants>
// struct VariantType;

// template <> struct VariantType<Variants::kString> { using Type = std::string; };
// template <> struct VariantType<Variants::kUint64> { using Type = uint64_t; };
// template <> struct VariantType<Variants::kInt64> { using Type = int64_t; };
// template <> struct VariantType<Variants::kDouble> { using Type = double; };
// template <> struct VariantType<Variants::kFloat> { using Type = float; };
// template <> struct VariantType<Variants::kBool> { using Type = bool; };
