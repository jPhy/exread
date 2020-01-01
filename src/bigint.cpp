#include "../exread/bigint.hpp"

namespace exread {

    /*
     *  operator==
     */
    bool BigInt::operator== (const BigInt& other) const
    {
        if (neg != other.neg)
            return false;

        size_t size = digits.size();

        if (size != other.digits.size())
            return false;

        for (size_t idx = 0; idx < size; ++size)
            if (digits[idx] != other.digits[idx])
                return false;

        return true;
    }

    /*
     *  operator>=
     */
    bool BigInt::operator>= (const BigInt& other) const
    {
        if ( neg && (!other.neg) )
            return false;

        else if ( (!neg) && other.neg )
            return true;

        else if ( (!neg) && (!other.neg) ) {
            size_t size = digits.size();

            if (size > other.digits.size())
                return true;
            else if (size < other.digits.size())
                return false;

            assert((size == other.digits.size()));

            if (size == 0)
                return true;

            for (size_t idx = size-1; ; --idx)
            {
                if (digits[idx] > other.digits[idx])
                    return true;
                else if (digits[idx] < other.digits[idx])
                    return false;

                if (idx == 0)
                    break;
            }

            assert(*this == other);

            return true;
        } else {
            return -other >= -*this;
        }

    }

    /*
     *  binary arithmetic operators
     */
    // helper function add
    template<int half_base_digits, typename base_int, base_int leading_mask>
    static std::vector<base_int> add(const std::vector<base_int>& n1, const std::vector<base_int>& n2)
    {
        assert(n1.size() >= n2.size());

        base_int overflow = 0;
        size_t idx;
        std::vector<base_int> result = n1;
        for (idx = 0; idx < n2.size(); ++idx)
        {
            overflow = result[idx] + n2[idx] + overflow;
            result[idx] = overflow & leading_mask; // remove high digits
            overflow >>= half_base_digits; // remove low digits
        }

        // further overflow after n2 depleted?
        if (overflow)
        {
            if (n1.size() == n2.size())
                result.push_back(overflow);
            else
            {
                for ( ; idx < n1.size(); ++idx)
                {
                    overflow += result[idx];
                    result[idx] = overflow & leading_mask; // remove high digits
                    overflow >>= half_base_digits; // remove low digits
                    if (overflow == 0)
                        break;
                }
                // last overflow
                if (overflow)
                    result.push_back(overflow);
            }
        }

        return result;
    }
    // helper function subtract
    template<int half_base_digits, typename base_int, base_int leading_mask>
    static std::vector<base_int> subtract(const std::vector<base_int>& n1, const std::vector<base_int>& n2)
    {
        assert(n1.size() >= n2.size());

        base_int overflow = 0;
        const base_int overflow_guard = 1 << half_base_digits;
        size_t idx;
        std::vector<base_int> result = n1;
        for (idx = 0; idx < n2.size(); ++idx)
        {
            overflow = result[idx] - (n2[idx] + overflow);
            if (overflow > overflow_guard) // overflow occurred
            {
                result[idx] = overflow + overflow_guard;
                overflow = 1;
            } else {
                result[idx] = overflow;
                overflow = 0;
            }
        }

        // further overflow after n2 depleted?
        if (n1.size() == n2.size())
            assert(overflow == 0); // function assumes n1 >= n2

        if (overflow)
        {
            for ( ; idx < n1.size(); ++idx)
            {
                overflow = result[idx] - overflow;
                if (overflow > overflow_guard) // overflow occurred
                {
                    result[idx] = overflow + overflow_guard;
                    overflow = 1;
                } else {
                    result[idx] = overflow;
                    overflow = 0;
                    break;
                }
            }
            // last overflow
            assert(overflow == 0); // function assumes n1 >= n2
        }

        // remove leading zeros
        while (result.size() > 0 && result.back() == 0)
            result.pop_back();

        return result;
    }

    BigInt operator+ (const BigInt& n1, const BigInt& n2)
    {
        if (n1.neg == n2.neg)
        {
            if (n1.digits.size() >= n2.digits.size())
                return {n1.neg, add<BigInt::half_base_digits,BigInt::base_int,BigInt::leading_mask>(n1.digits, n2.digits)};
            else
                return {n1.neg, add<BigInt::half_base_digits,BigInt::base_int,BigInt::leading_mask>(n2.digits, n1.digits)};
        } else {
            if (n1.neg)
            {
                if (-n1 >= n2)
                    return {true, subtract<BigInt::half_base_digits,BigInt::base_int,BigInt::leading_mask>(n1.digits, n2.digits)};
                else
                    return {false, subtract<BigInt::half_base_digits,BigInt::base_int,BigInt::leading_mask>(n2.digits, n1.digits)};
            } else {
                if (n1 >= -n2)
                    return {false, subtract<BigInt::half_base_digits,BigInt::base_int,BigInt::leading_mask>(n1.digits, n2.digits)};
                else
                    return {true, subtract<BigInt::half_base_digits,BigInt::base_int,BigInt::leading_mask>(n2.digits, n1.digits)};
            }
        };
    }
    BigInt operator- (const BigInt& n1, const BigInt& n2)
    {
        return n1 + (-n2);
    }

    BigInt operator* (const BigInt& n1, const BigInt& n2)
    {
        using base_int = BigInt::base_int;
        std::vector<base_int> res_digits;

        for (size_t idx1 = 0; idx1 < n1.digits.size(); ++idx1)
            for (size_t idx2 = 0; idx2 < n2.digits.size(); ++idx2)
            {
                base_int tmp = n1.digits[idx1] * n2.digits[idx2];
                std::vector<base_int> tmp_digits(idx1+idx2+2);

                *(tmp_digits.end()-2) = tmp&BigInt::leading_mask;
                tmp >>= BigInt::half_base_digits;
                if (tmp == 0)
                    tmp_digits.pop_back();
                else
                    tmp_digits.back() = tmp;

                if (res_digits.size() >= tmp_digits.size())
                    res_digits = add<BigInt::half_base_digits,BigInt::base_int,BigInt::leading_mask>(res_digits, tmp_digits);
                else
                    res_digits = add<BigInt::half_base_digits,BigInt::base_int,BigInt::leading_mask>(tmp_digits, res_digits);
            }

        return {n1.neg^n2.neg, std::move(res_digits)};
    }

    BigInt operator/ (const BigInt& n1, const BigInt& n2)
    {

        using base_int = BigInt::base_int;

        // handle division by zero
        if (n2.digits.size() == 0)
            throw std::invalid_argument("Division by BigInt(0)");

        // handle sign
        if (n1.neg)
        {
            if (n2.neg)
                return (-n1) / (-n2);
            else
                return -( (-n1)/n2 );
        } else if (n2.neg) {
            return -( n1/(-n2) );
        }

        // handle zero result
        if (  (n2 > n1) || (n1.digits.size() == 0)  )
            return 0;

        assert(n1.digits.size() >= n2.digits.size());
        assert(n1.digits.size() >= 1);

        // division as with base_int
        if (n1.digits.size() == 1)
        {
            assert(n2.digits.size() == 1);
            return n1.digits[0] / n2.digits[0];


        // division with multiple digits
        } else {

            assert(n1.digits.size() >= 2);

            // compute a lower bound as "<hightes two digits of n1> / (<hightes digit of n2> + 1)"
            const base_int tmp_num = (n1.digits.back() << BigInt::half_base_digits) | *(n1.digits.end()-2);
            const base_int tmp_den = n2.digits.back() + 1;
            const base_int tmp_res = (tmp_num / tmp_den);

            // correct shift introduced by only considering the leading digits in "tmp_res"
            const size_t number_of_leading_zeros_plus_one = n1.digits.size() - n2.digits.size() + 1;
            std::vector<base_int> lower_bound_for_res_digits(number_of_leading_zeros_plus_one);
            lower_bound_for_res_digits.back() = tmp_res >> BigInt::half_base_digits;
            if (lower_bound_for_res_digits.size() > 1)
                *(lower_bound_for_res_digits.end()-2) = tmp_res & BigInt::leading_mask;

            // remove leading zeros
            while (lower_bound_for_res_digits.size() > 0 && lower_bound_for_res_digits.back() == 0)
                lower_bound_for_res_digits.pop_back();

            // The division result is definitely larger than zero since we catch that case right away in "if (n2 > n1)".
            if (lower_bound_for_res_digits.empty())
                lower_bound_for_res_digits.push_back(1);

            const BigInt lower_bound_for_res(false,std::move(lower_bound_for_res_digits));
            return lower_bound_for_res + (n1-n2*lower_bound_for_res) / n2;

        }

    }

}
