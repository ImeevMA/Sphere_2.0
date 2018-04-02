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
    BigInt(int64_t num = 0);
    BigInt(const BigInt &other);
    BigInt(BigInt &&other);
    ~BigInt() { delete[] arr; }

    BigInt &operator=(const BigInt &other);
    BigInt &operator=(BigInt &&other);
    BigInt operator-() const;

    BigInt operator+(const BigInt &other) const;
    BigInt operator-(const BigInt &other) const;
    BigInt operator*(const BigInt &other) const;
    BigInt operator/(const BigInt &other) const;
    bool operator>(const BigInt &other) const;
    bool operator<(const BigInt &other) const;
    bool operator>=(const BigInt &other) const;
    bool operator<=(const BigInt &other) const;
    bool operator==(const BigInt &other) const;
    bool operator!=(const BigInt &other) const;

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

BigInt::BigInt(int64_t num)
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

BigInt::BigInt(BigInt &&other)
{
    max_size = other.max_size;
    cur_size = other.cur_size;
    is_positive = other.is_positive;
    arr = other.arr;
    other.arr = nullptr;
}

BigInt &BigInt::operator=(const BigInt &other)
{
    max_size = other.max_size;
    cur_size = other.cur_size;
    is_positive = other.is_positive;
    delete[] arr;
    arr = new Number[max_size]();
    for(size_t i = 0; i < cur_size; ++i) arr[i] = other.arr[i];
    return *this;
}

BigInt &BigInt::operator=(BigInt &&other)
{
    max_size = other.max_size;
    cur_size = other.cur_size;
    is_positive = other.is_positive;
    delete[] arr;
    arr = other.arr;
    other.arr = nullptr;
    return *this;
}

BigInt BigInt::operator+(const BigInt &other) const
{
    if(cur_size == 0 && other.cur_size == 0) return BigInt(0);
    if(is_positive != other.is_positive) return *this - (-other);
    const BigInt *a = &*this;
    const BigInt *b = &other;
    if(other.cur_size > cur_size) {
        a = &other;
        b = &*this;
    }
    BigInt tmp(*a);
    for(size_t i = 0; i < b->cur_size; ++i) tmp.arr[i] += b->arr[i];
    tmp.normalize_inc();
    return tmp;
}

BigInt BigInt::operator*(const BigInt &other) const
{
    if(cur_size == 0 || other.cur_size == 0) return BigInt(0);
    BigInt tmp = BigInt();
    BigInt shifted_tmp(*this);
    shifted_tmp.is_positive = true;
    for(size_t i = 0; i < other.cur_size; ++i) {
        BigInt loc_tmp(shifted_tmp);
        for(size_t j = i; j < loc_tmp.cur_size; ++j) loc_tmp.arr[j] *= other.arr[i];
        tmp = loc_tmp + tmp;
        shifted_tmp.shift();
    }
    tmp.is_positive = !(is_positive ^ other.is_positive);
    return tmp;
}

BigInt BigInt::operator-() const
{
    BigInt tmp(*this);
    tmp.is_positive = !is_positive;
    return tmp;
}

BigInt BigInt::operator-(const BigInt &other) const
{
    if(cur_size == 0 && other.cur_size == 0) return BigInt(0);
    if(is_positive != other.is_positive) return *this + (-other);
    if(cur_size > other.cur_size) {
        BigInt tmp(*this);
        for(size_t i = 0; i < other.cur_size; ++i) tmp.arr[i] -= other.arr[i];
        tmp.normalize_dec();
        return tmp;
    }
    BigInt tmp(other);
    for(size_t i = 0; i < other.cur_size; ++i) tmp.arr[i] = -tmp.arr[i];
    for(size_t i = 0; i < cur_size; ++i) tmp.arr[i] += arr[i];
    tmp.normalize_dec();
    return tmp;
}

bool BigInt::operator<(const BigInt &other) const
{
    return (*this != other) && (other - *this).is_positive;
}

bool BigInt::operator<=(const BigInt &other) const
{
    return (*this < other) || (*this == other);
}

bool BigInt::operator>=(const BigInt &other) const
{
    return (*this > other) || (*this == other);
}

bool BigInt::operator>(const BigInt &other) const
{
    return (*this != other) && (*this - other).is_positive;
}

bool BigInt::operator==(const BigInt &other) const
{
    return !((*this - other).cur_size);
}

bool BigInt::operator!=(const BigInt &other) const
{
    return !(*this == other);
}

BigInt BigInt::operator/(const BigInt &other) const
{
    if(*this == 0) return BigInt(0);
    BigInt a(*this), b(other), c(0);
    a.is_positive = true;
    b.is_positive = true;
    a >= b;
    while(a >= b) {
        BigInt tmp(b);
        BigInt shifted_tmp(b);
        BigInt tmp_c(1);
        int i = 0;
        shifted_tmp.shift();
        while(a > shifted_tmp) {
            tmp = shifted_tmp;
            tmp_c.shift();
            shifted_tmp.shift();
        }
        while(a >= tmp) {
            a = a - tmp;
            ++i;
        }
        c = c + tmp_c * i;
    }
    c.is_positive = !(is_positive ^ other.is_positive);
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
    int i = 1;
    while(i <= cur_size) {
        if(cur_size == max_size) expansion();
        if(arr[i - 1] >= BASE) {
            arr[i] += arr[i - 1] / BASE;
            arr[i - 1] = arr[i - 1] % BASE;
        }
        if(arr[cur_size] > 0) ++cur_size;
        ++i;
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