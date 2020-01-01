#include <algorithm> // std::max
#include <cassert> // assert
#include <cstdio> // std::size_t
#include <limits> // std::numeric_limits
#include <stdexcept> // std::invalid_argument
#include <type_traits> // std::enable_if
#include <utility> // std::move
#include <vector> // std::vector

namespace exread {

    using std::size_t;

    class BigInt
    {
        private:

            using base_int = unsigned int;
            static constexpr int base_radix = std::numeric_limits<base_int>::radix;
            static constexpr int base_digits = std::numeric_limits<base_int>::digits;
            static constexpr int half_base_digits = base_digits / 2;
            static constexpr base_int leading_mask = (1 << half_base_digits) - 1;
            static_assert(half_base_digits > 0, "BigInt must have an integral type with at least 2 digits as base_int.");

            bool neg; // sign bit
            std::vector<base_int> digits;

            // private constructors from sign and digits
            BigInt(const bool& neg, const std::vector<base_int>& digits) : neg(digits.size()>0?neg:false), digits(digits) {}; // copy
            BigInt(const bool& neg, std::vector<base_int>&& digits) : neg(digits.size()>0?neg:false), digits(std::move(digits)) {}; // move

        public:

            /*
             *  Constructors
             */

            // default
            BigInt() : neg(false), digits() {};

            // from builtin integral type
            template<typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
            BigInt(T n) : neg(n<0)
            {
                static_assert(std::numeric_limits<T>::radix == base_radix, "BigInt must be constructed from integral type with the same radix as the base_int.");

                // sign has been taken care of in initializer list
                if (n < 0)
                    n = -n;

                // fill digits
                while (n > 0)
                {
                    digits.push_back(
                                         base_int(n) & leading_mask // strip leading digits
                                    );
                    n >>= half_base_digits; // remove lower digits of 'n' (which have just been added to 'digits')
                };

            };

            // from string
            BigInt(const std::string& n) : neg(n.size()>0 ? n[0] == '-' : false)
            {
                BigInt res, base10 = 1;

                for (auto digit = n.crbegin(); digit != (neg ? n.crend()-1 : n.crend()); ++digit)
                {
                    char tmp = *digit - 48;
                    if (tmp < 0 || tmp > 9)
                        throw std::invalid_argument("BigInt(\"" + n + "\")");
                    res = res + tmp * base10;
                    base10 = base10 * 10;
                }

                this->digits = std::move(res.digits);
            };

        /*
         *  comparison operators
         */
        bool operator== (const BigInt& other) const;
        bool operator!= (const BigInt& other) const { return !(*this == other); }
        bool operator>= (const BigInt& other) const;
        bool operator<  (const BigInt& other) const { return !(*this >= other); }
        bool operator<= (const BigInt& other) const { return  ( other >= *this); }
        bool operator>  (const BigInt& other) const { return !(*this <= other); }


        /*
         *  unary arithmetic operators
         */
        BigInt operator+() const {  return *this;  };
        BigInt operator-() const {  return {!neg, digits};  };

        /*
         *  binary arithmetic operators
         */
        friend BigInt operator+ (const BigInt& n1, const BigInt& n2);
        friend BigInt operator- (const BigInt& n1, const BigInt& n2);
        friend BigInt operator* (const BigInt& n1, const BigInt& n2);
        friend BigInt operator/ (const BigInt& n1, const BigInt& n2);

    };

}
