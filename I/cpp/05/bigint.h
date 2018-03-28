using Number = signed char;

constexpr Number BASE = 10;
constexpr size_t SIZE = 32;

class BigInt
{
    Number *arr = nullptr;
    size_t max_size = SIZE;
    size_t cur_size = 0;
    bool is_positive = true;

    void shift();
    void normalize_dec();
    void normalize_inc();
    void expansion(size_t delta = SIZE);
public:
    BigInt(int num = 0);
    BigInt(const BigInt &other);
    ~BigInt() { delete[] arr; }

    BigInt &operator=(const BigInt &other);
    BigInt operator-() const;

    friend BigInt operator+(const BigInt &first, const BigInt &second);
    friend BigInt operator-(const BigInt &first, const BigInt &second);
    friend BigInt operator*(const BigInt &first, const BigInt &second);
    friend BigInt operator/(const BigInt &first, const BigInt &second);
    friend bool operator>(const BigInt &first, const BigInt &second);
    friend bool operator<(const BigInt &first, const BigInt &second);
    friend bool operator==(const BigInt &first, const BigInt &second);
    friend bool operator!=(const BigInt &first, const BigInt &second);
    friend std::ostream& operator<<(std::ostream& out, const BigInt& value);
};

std::ostream& operator<<(std::ostream& out, const BigInt& value)
{
    if(value.cur_size == 0) {
        out << 0;
    } else {
        if(!value.is_positive) out << "-";
        for(size_t i = value.cur_size; i > 0; --i) { out << int(value.arr[i - 1]); }
    }
}

BigInt::BigInt(int num)
{
    arr = new Number[max_size]();
    if(num < 0) {
        is_positive = false;
        arr[cur_size] = num % BASE;
        if (arr[cur_size] < 0) arr[cur_size] = -arr[cur_size];
        ++cur_size;
        num = -(num / BASE);
    }
    for(; num != 0 && cur_size < max_size; ++cur_size, num /= BASE) {
        arr[cur_size] = num % BASE;
    }
}

BigInt::BigInt(const BigInt &other)
{
    max_size = other.max_size;
    cur_size = other.cur_size;
    is_positive = other.is_positive;
    arr = new Number[max_size]();
    for(size_t i = 0; i < cur_size; ++i) arr[i] = other.arr[i];
}

BigInt &BigInt::operator=(const BigInt &other)
{
    max_size = other.max_size;
    cur_size = other.cur_size;
    is_positive = other.is_positive;
    delete[] arr;
    arr = new Number[max_size]();
    for(size_t i = 0; i < cur_size; ++i) arr[i] = other.arr[i];
}

BigInt operator+(const BigInt &first, const BigInt &second)
{
    if(first.is_positive != second.is_positive) return first - (-second);
    const BigInt *a = &first;
    const BigInt *b = &second;
    if(second.cur_size > first.cur_size) {
        a = &second;
        b = &first;
    }
    BigInt tmp(*a);
    for(size_t i = 0; i < b->cur_size; ++i) tmp.arr[i] += b->arr[i];
    tmp.normalize_inc();
    return tmp;
}

BigInt operator*(const BigInt &first, const BigInt &second)
{
    BigInt tmp = BigInt();
    BigInt shifted_tmp(first);
    shifted_tmp.is_positive = true;
    for(size_t i = 0; i < second.cur_size; ++i) {
        BigInt loc_tmp(shifted_tmp);
        for(size_t j = i; j < loc_tmp.cur_size; ++j) loc_tmp.arr[j] *= second.arr[i];
        tmp = loc_tmp + tmp;
        shifted_tmp.shift();
    }
    tmp.is_positive = !(first.is_positive ^ second.is_positive);
    return tmp;
}

BigInt BigInt::operator-() const
{
    BigInt tmp(*this);
    tmp.is_positive = !is_positive;
    return tmp;
}

BigInt operator-(const BigInt &first, const BigInt &second)
{
    if(first.is_positive != second.is_positive) return first + (-second);
    if(first.cur_size > second.cur_size) {
        BigInt tmp(first);
        for(size_t i = 0; i < second.cur_size; ++i) tmp.arr[i] -= second.arr[i];
        tmp.normalize_dec();
        return tmp;
    }
    BigInt tmp(second);
    for(size_t i = 0; i < second.cur_size; ++i) tmp.arr[i] = -tmp.arr[i];
    for(size_t i = 0; i < first.cur_size; ++i) tmp.arr[i] += first.arr[i];
    tmp.normalize_dec();
    return tmp;
}

bool operator<(const BigInt &first, const BigInt &second)
{
    return (first != second) && (second - first).is_positive;
}

bool operator<=(const BigInt &first, const BigInt &second)
{
    return (first < second) || (first == second);
}

bool operator>=(const BigInt &first, const BigInt &second)
{
    return (first > second) || (first == second);
}

bool operator>(const BigInt &first, const BigInt &second)
{
    return (first != second) && (first - second).is_positive;
}

bool operator==(const BigInt &first, const BigInt &second)
{
    return !((first - second).cur_size);
}

bool operator!=(const BigInt &first, const BigInt &second)
{
    return !(first == second);
}

BigInt operator/(const BigInt &first, const BigInt &second)
{
    if(first == 0) return BigInt(0);
    BigInt a(first), b(second), c(0);
    a.is_positive = true;
    b.is_positive = true;
    while(a >= b) {
        BigInt tmp(b);
        BigInt shifted_tmp(b);
        while(shifted_tmp < a) {
            tmp = shifted_tmp;
            shifted_tmp.shift();
        }
        if(a.cur_size == tmp.cur_size) {
            Number mult = a.arr[a.cur_size - 1] / tmp.arr[tmp.cur_size - 1];
            if(a < tmp * mult) --mult;
            a = a - tmp * mult;
            c = c * BASE + mult;
        } else {
            int mult = (a.arr[a.cur_size - 1] * BASE + a.arr[a.cur_size - 2]) / tmp.arr[tmp.cur_size - 1];
            if(a < tmp * mult) --mult;
            a = a - tmp * mult;
            a.normalize_dec(); // Why? Shouldn't be any problem here!!!!
            // std::cout << "|" << mult << "|" << c << "|" << a << std::endl;
            c = (c * BASE) + mult;
            // std::cout << c << std::endl;
        }
    }
    c.is_positive = !(first.is_positive ^ second.is_positive);
    return c;
}

void BigInt::normalize_dec()
{
    while(cur_size > 1 && arr[cur_size - 1] == 0) --cur_size;
    if(arr[cur_size - 1] < 0) {
        for(size_t i = 0; i < cur_size; ++i) {
            arr[i] = -arr[i];
        }
        is_positive = !is_positive;
    }
    for(size_t i = 0; i < cur_size - 1; ++i) {
        if(arr[i] < 0) {
            arr[i] += BASE;
            --arr[i + 1];
        }
    }
    while(cur_size > 0 && arr[cur_size - 1] == 0) --cur_size;
}

void BigInt::normalize_inc()
{
    for(size_t i = 1; i < cur_size; ++i)
    {
        if(arr[i - 1] >= BASE) {
            arr[i] += arr[i - 1] / BASE;
            arr[i - 1] %= BASE;
        }
    }
    if(cur_size == max_size && arr[cur_size - 1] >= BASE) {
        expansion();
        arr[cur_size] += arr[cur_size - 1] / BASE;
        arr[cur_size] %= BASE;
        ++cur_size;
    }
}

void BigInt::expansion(size_t delta)
{
    max_size += delta;
    Number *tmp_arr = new Number[max_size]();
    for(int i = 0; i < cur_size; ++i) tmp_arr[i] = arr[i];
    delete[] arr;
    arr = tmp_arr;
}

void BigInt::shift()
{
    if(cur_size == max_size) expansion();
    ++cur_size;
    for(size_t i = cur_size - 1; i > 0; --i) arr[i] = arr[i - 1];
    arr[0] = 0;
}