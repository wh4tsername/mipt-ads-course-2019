#pragma once

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

int Power(int value, int power) {
  int result = 1;

  while (power > 0) {
    if (power % 2 == 1) {
      result *= value;
    }

    value *= value;
    power /= 2;
  }

  return result;
}

class BigInteger {
 public:
  typedef uint16_t base_t;
  typedef uint32_t ext_base_t;

  BigInteger(const BigInteger& other);
  BigInteger(BigInteger&& other) noexcept;

  BigInteger(const std::string& number_str);
  template <class IntegerType = int>
  BigInteger(IntegerType number = 0);
  explicit operator bool();

  ~BigInteger() = default;

  BigInteger& operator=(const BigInteger& other);
  BigInteger& operator=(BigInteger&& other) noexcept;

  BigInteger& operator+=(const BigInteger& value);
  BigInteger& operator-=(const BigInteger& value);
  BigInteger& operator*=(const BigInteger& value);
  BigInteger& operator/=(const BigInteger& value);
  BigInteger& operator%=(const BigInteger& value);

  BigInteger& operator++();
  BigInteger operator++(int);

  BigInteger& operator--();
  BigInteger operator--(int);

  friend BigInteger operator+(const BigInteger& value);
  friend BigInteger operator-(const BigInteger& value);

  friend BigInteger operator+(const BigInteger& lhs, const BigInteger& rhs);
  friend BigInteger operator-(const BigInteger& lhs, const BigInteger& rhs);

  friend BigInteger operator*(const BigInteger& lhs, const BigInteger& rhs);
  friend BigInteger operator/(const BigInteger& lhs, const BigInteger& rhs);
  friend BigInteger operator%(const BigInteger& lhs, const BigInteger& rhs);

  friend BigInteger abs(const BigInteger& value);

  friend bool operator<(const BigInteger& lhs, const BigInteger& rhs);
  friend bool operator>(const BigInteger& lhs, const BigInteger& rhs);
  friend bool operator>=(const BigInteger& lhs, const BigInteger& rhs);
  friend bool operator<=(const BigInteger& lhs, const BigInteger& rhs);
  friend bool operator==(const BigInteger& lhs, const BigInteger& rhs);
  friend bool operator!=(const BigInteger& lhs, const BigInteger& rhs);

  friend std::ostream& operator<<(std::ostream& os, const BigInteger& value);
  friend std::istream& operator>>(std::istream& is, BigInteger& value);

  std::string toString() const;

 private:
  static const base_t BASE = 10;

  std::vector<base_t> buffer_;
  bool is_negative_;

  base_t operator[](size_t index) const;
  base_t operator[](size_t index);
  const base_t& At(size_t index) const;
  base_t& At(size_t index);

  static BigInteger SumPositives(const BigInteger& lhs, const BigInteger& rhs);
  static BigInteger SubtractPositives(const BigInteger& lhs,
                                      const BigInteger& rhs);
  static BigInteger MultiplyPositives(const BigInteger& lhs,
                                      const BigInteger& rhs);
  static BigInteger DividePositives(const BigInteger& lhs,
                                    const BigInteger& rhs);

  template <class IntegerType = int>
  static std::string NumberToString(IntegerType number);
  static void AddLeadingZeros(std::string& number_string, int segment_length);

  static void DeleteLeadingZeros(BigInteger& number);
  void Clear();
  bool IsNegative() const;
  size_t Size() const;

  template <class IntegerType>
  static std::vector<base_t> ConvertToBase(IntegerType number);
  template <class IntegerType>
  static size_t NumberLength(IntegerType number);
  static bool HaveSameSign(const BigInteger& first, const BigInteger& second);

  template <class IntegerType>
  void FillBufferFromNumber(IntegerType number);
  void FillBufferWithZeros(size_t required_size);
  void FillBufferFromString(const std::string& number_str);
};

BigInteger::BigInteger(const BigInteger& other) : buffer_(other.buffer_) {
  is_negative_ = other.is_negative_;
}

BigInteger::BigInteger(BigInteger&& other) noexcept {
  std::swap(buffer_, other.buffer_);
  is_negative_ = other.is_negative_;
}

BigInteger& BigInteger::operator=(const BigInteger& other) {
  if (this != &other) {
    is_negative_ = other.is_negative_;
    buffer_.clear();
    for (size_t i = 0; i < other.Size(); ++i) {
      buffer_.emplace_back(other.buffer_[i]);
    }
  }
  return *this;
}

BigInteger& BigInteger::operator=(BigInteger&& other) noexcept {
  is_negative_ = other.is_negative_;
  if (this != &other) {
    buffer_.clear();
  }
  std::swap(buffer_, other.buffer_);
  return *this;
}

template <class IntegerType>
BigInteger::BigInteger(IntegerType number) {
  if (number >= 0) {
    is_negative_ = false;
  } else {
    is_negative_ = true;
    number *= -1;
  }

  this->FillBufferFromNumber(number);
}

template <class IntegerType>
size_t BigInteger::NumberLength(IntegerType number) {
  if (number < 0) {
    number = -number;
  }
  size_t length = 0;
  do {
    number /= 10;
    ++length;
  } while (number != 0);
  return length;
}

BigInteger::BigInteger(const std::string& number_str) {
  this->FillBufferFromString(number_str);
}

static void CopyNumberStrAndLeadingZerosToStr(std::string& str,
                                              size_t number_of_leading_zeros,
                                              const std::string& number_str,
                                              size_t string_size,
                                              size_t beginning,
                                              bool is_negative) {
  for (size_t i = 0; i < number_of_leading_zeros; ++i) {
    str[i] = '0';
  }
  for (size_t i = beginning, j = static_cast<size_t>(is_negative);
       j <= string_size; ++i, ++j) {
    str[i] = number_str[j];
  }
}

static void SplitStrIntoSegmentsToConvertThemIntoDigits(
    std::vector<BigInteger::base_t>& stack_of_digits, const std::string& str,
    const size_t SEGMENT_LENGTH, const size_t NUMBER_OF_SEGMENTS) {
  size_t beginning_of_segment = 0;
  for (size_t i = 1; i <= NUMBER_OF_SEGMENTS; ++i) {
    int k = 0;
    std::string current_segment;
    current_segment.reserve(SEGMENT_LENGTH);
    for (size_t j = beginning_of_segment;
         j < beginning_of_segment + SEGMENT_LENGTH; ++j) {
      current_segment[k++] = str[j];
    }
    current_segment[k] = '\0';
    BigInteger::base_t current_digit = 0;
    for (int ind = 0; ind < static_cast<int>(SEGMENT_LENGTH); ++ind) {
      current_digit +=
          static_cast<BigInteger::base_t>(current_segment[ind] - '0') *
          static_cast<BigInteger::base_t>(
              Power(10, static_cast<int>(SEGMENT_LENGTH) - ind - 1));
    }
    stack_of_digits.emplace_back(current_digit);
    beginning_of_segment += SEGMENT_LENGTH;
  }
}

void BigInteger::FillBufferFromString(const std::string& number_str) {
  // the main purpose of this function is to split initial string into segments
  // to push than into buffer_ after like that 64'987'132'658'972'675
  //                                        (if BigInteger::Base equals 1000)

  is_negative_ = (number_str[0] == '-');
  buffer_.clear();

  // this part is a bunch of formulas
  size_t string_size = number_str.size();
  size_t unsigned_string_size = string_size - static_cast<size_t>(is_negative_);
  const size_t SEGMENT_LENGTH = NumberLength(BASE) - 1;
  const size_t NUMBER_OF_SEGMENTS =
      unsigned_string_size / SEGMENT_LENGTH +
      ((unsigned_string_size % SEGMENT_LENGTH == 0) ? 0 : 1);
  std::string str;
  str.reserve(NUMBER_OF_SEGMENTS * SEGMENT_LENGTH + 1);

  size_t number_of_leading_zeros =
      SEGMENT_LENGTH - unsigned_string_size +
      (unsigned_string_size / SEGMENT_LENGTH) * SEGMENT_LENGTH;
  if (number_of_leading_zeros == SEGMENT_LENGTH) {
    number_of_leading_zeros = 0;
  }
  size_t beginning = number_of_leading_zeros;

  // copy number
  CopyNumberStrAndLeadingZerosToStr(str, number_of_leading_zeros, number_str,
                                    string_size, beginning, is_negative_);

  // stack stores digits to pop them into buffer_
  std::vector<base_t> stack_of_digits;
  SplitStrIntoSegmentsToConvertThemIntoDigits(
      stack_of_digits, str, SEGMENT_LENGTH, NUMBER_OF_SEGMENTS);

  // filling buffer_ from digit stack
  while (!stack_of_digits.empty()) {
    buffer_.emplace_back(stack_of_digits.back());
    stack_of_digits.pop_back();
  }
}

template <class IntegerType>
std::vector<BigInteger::base_t> BigInteger::ConvertToBase(IntegerType number) {
  std::vector<base_t> array_with_digits;

  do {
    array_with_digits.emplace_back(number % BASE);
    number /= BASE;
  } while (number != 0);

  return array_with_digits;
}

template <class IntegerType>
void BigInteger::FillBufferFromNumber(IntegerType number) {
  buffer_.clear();
  buffer_ = ConvertToBase(number);
}

void BigInteger::FillBufferWithZeros(const size_t required_size) {
  buffer_.clear();
  for (size_t i = 0; i < required_size; ++i) {
    this->buffer_.emplace_back(0);
  }
}

bool BigInteger::IsNegative() const { return is_negative_; }

bool BigInteger::HaveSameSign(const BigInteger& first,
                              const BigInteger& second) {
  return first.IsNegative() == second.IsNegative();
}

void BigInteger::DeleteLeadingZeros(BigInteger& number) {
  size_t index = number.buffer_.size();
  while (!number.buffer_.empty() && number.buffer_[index - 1] == 0) {
    number.buffer_.pop_back();
    --index;
  }
  if (number.buffer_.empty()) {
    number.buffer_.emplace_back(0);
  }
}

BigInteger BigInteger::SumPositives(const BigInteger& lhs,
                                    const BigInteger& rhs) {
  BigInteger unsigned_lhs = abs(lhs);
  BigInteger unsigned_rhs = abs(rhs);

  const size_t lhs_size = unsigned_lhs.Size();
  const size_t rhs_size = unsigned_rhs.Size();
  const size_t max_size = std::max(lhs_size, rhs_size);

  BigInteger result;
  result.Clear();

  base_t transfer = 0;
  for (size_t i = 0; i < max_size; ++i) {
    base_t new_digit = unsigned_lhs[i] + unsigned_rhs[i] + transfer;
    result.buffer_.emplace_back(new_digit % BASE);
    transfer = new_digit / BASE;
  }
  if (transfer != 0) {
    result.buffer_.emplace_back(transfer);
  }

  return result;
}

BigInteger BigInteger::SubtractPositives(const BigInteger& lhs,
                                         const BigInteger& rhs) {
  BigInteger unsigned_lhs = abs(lhs);
  BigInteger unsigned_rhs = abs(rhs);

  if (unsigned_lhs < unsigned_rhs) {
    return -(SubtractPositives(unsigned_rhs, unsigned_lhs));
  }

  const size_t lhs_size = unsigned_lhs.Size();
  const size_t rhs_size = unsigned_rhs.Size();
  const size_t max_size = std::max(lhs_size, rhs_size);

  BigInteger result;
  result.Clear();

  base_t transfer = 0;
  for (size_t i = 0; i < max_size; ++i) {
    int new_digit = unsigned_lhs[i] - transfer - unsigned_rhs[i];
    transfer = static_cast<base_t>(new_digit < 0);
    if (transfer != 0) {
      new_digit += BASE;
    }
    result.buffer_.emplace_back(static_cast<base_t>(new_digit));
  }

  BigInteger::DeleteLeadingZeros(result);

  return result;
}

BigInteger operator-(const BigInteger& value) {
  BigInteger new_value = value;
  new_value.is_negative_ = !value.is_negative_;
  return new_value;
}

BigInteger operator+(const BigInteger& value) { return value; }

BigInteger operator+(const BigInteger& lhs, const BigInteger& rhs) {
  BigInteger result;

  if (BigInteger::HaveSameSign(lhs, rhs)) {
    result = BigInteger::SumPositives(lhs, rhs);
    result.is_negative_ = lhs.IsNegative();
  } else if (lhs.IsNegative()) {
    result = BigInteger::SubtractPositives(rhs, lhs);
  } else {
    result = BigInteger::SubtractPositives(lhs, rhs);
  }

  return result;
}

BigInteger operator-(const BigInteger& lhs, const BigInteger& rhs) {
  BigInteger result;

  if (BigInteger::HaveSameSign(lhs, rhs)) {
    result = lhs + (-rhs);
  } else if (lhs.IsNegative()) {
    result = -(-lhs + rhs);
  } else {
    result = -(rhs - lhs);
  }

  return result;
}

BigInteger& BigInteger::operator+=(const BigInteger& value) {
  return *this = (*this + value);
}

BigInteger& BigInteger::operator-=(const BigInteger& value) {
  return *this = (*this - value);
}

BigInteger& BigInteger::operator++() { return (*this += 1); }

BigInteger BigInteger::operator++(int) {
  *this += 1;
  return *this - 1;
}

BigInteger& BigInteger::operator--() { return (*this -= 1); }

BigInteger BigInteger::operator--(int) {
  *this -= 1;
  return *this + 1;
}

BigInteger BigInteger::MultiplyPositives(const BigInteger& lhs,
                                         const BigInteger& rhs) {
  BigInteger result;

  result.FillBufferWithZeros(lhs.buffer_.size() + rhs.buffer_.size());

  const BigInteger& lhs_less = (abs(lhs) > abs(rhs)) ? abs(rhs) : abs(lhs);
  const BigInteger& rhs_greater = (abs(lhs) > abs(rhs)) ? abs(lhs) : abs(rhs);

  for (size_t i = 0; i < lhs_less.buffer_.size(); ++i) {
    ext_base_t transfer = 0;
    for (size_t j = 0; j < rhs_greater.buffer_.size() || transfer != 0; ++j) {
      ext_base_t current_composition =
          result[i + j] + lhs_less[i] * rhs_greater[j] + transfer;
      result.At(i + j) =
          static_cast<base_t>(current_composition % BigInteger::BASE);
      transfer = static_cast<base_t>(current_composition / BigInteger::BASE);
    }
  }

  BigInteger::DeleteLeadingZeros(result);

  return result;
}

BigInteger operator*(const BigInteger& lhs, const BigInteger& rhs) {
  BigInteger result;

  result = BigInteger::MultiplyPositives(lhs, rhs);
  result.is_negative_ =
      result != 0 ? !BigInteger::HaveSameSign(lhs, rhs) : false;

  return result;
}

BigInteger& BigInteger::operator*=(const BigInteger& value) {
  return *this = (*this * value);
}

static int FindQuotientWithBinarySearch(const BigInteger& unsigned_lhs,
                                        const BigInteger& unsigned_rhs,
                                        const int BASE) {
  int quotient = 0, left_border = 0, right_border = BASE;
  while (left_border <= right_border) {
    int middle = (left_border + right_border) / 2;
    BigInteger temp = unsigned_rhs * middle;
    if (temp <= unsigned_lhs) {
      quotient = middle;
      left_border = middle + 1;
    } else {
      right_border = middle - 1;
    }
  }
  return quotient;
}

BigInteger BigInteger::DividePositives(const BigInteger& lhs,
                                       const BigInteger& rhs) {
  BigInteger result = 0;
  BigInteger unsigned_lhs = abs(lhs);
  BigInteger unsigned_rhs = abs(rhs);

  /*
   *     unsigned_lhs        | unsigned_rhs
   * - quotient * rhs        |_____________
   *   ______________        | result
   *          current
   */

  BigInteger current;
  size_t index = 1;
  while (index <= unsigned_lhs.Size()) {
    if (!(current == 0 && unsigned_lhs[unsigned_lhs.Size() - index] == 0)) {
      bool more_than_one_digit_required = false;
      while (current < unsigned_rhs) {
        if (index <= unsigned_lhs.Size()) {
          current = current * BASE + unsigned_lhs[unsigned_lhs.Size() - index];
          ++index;
          if (more_than_one_digit_required) {
            result *= BASE;
          }
          more_than_one_digit_required = true;
        } else {
          break;
        }
      }
    } else {
      while (unsigned_lhs[unsigned_lhs.Size() - index] == 0) {
        if (index > unsigned_lhs.Size()) {
          return result;
        }
        result *= BASE;
        ++index;
      }
      continue;
    }

    int quotient = FindQuotientWithBinarySearch(current, unsigned_rhs, BASE);

    current -= unsigned_rhs * quotient;
    result = result * BASE + quotient;
  }

  return result;
}

BigInteger operator/(const BigInteger& lhs, const BigInteger& rhs) {
  if (rhs == 0) {
    std::cerr << "Division by zero" << std::endl;
  }

  BigInteger result = BigInteger::DividePositives(lhs, rhs);
  result.is_negative_ =
      result != 0 ? !BigInteger::HaveSameSign(lhs, rhs) : false;

  return result;
}

BigInteger& BigInteger::operator/=(const BigInteger& value) {
  return *this = (*this / value);
}

BigInteger operator%(const BigInteger& lhs, const BigInteger& rhs) {
  BigInteger result = lhs - (lhs / rhs) * rhs;
  //  if (lhs.IsNegative() && !rhs.IsNegative()) {
  //    result -= rhs;
  //  }
  return result;
}

BigInteger& BigInteger::operator%=(const BigInteger& value) {
  return *this = (*this % value);
}

BigInteger::base_t BigInteger::operator[](size_t index) {
  if (index < buffer_.size()) {
    return buffer_[index];
  } else {
    return 0;
  }
}

BigInteger::base_t BigInteger::operator[](size_t index) const {
  if (index < buffer_.size()) {
    return buffer_[index];
  } else {
    return 0;
  }
}

const BigInteger::base_t& BigInteger::At(size_t index) const {
  if (index >= buffer_.size()) {
    std::cerr << "Out of range" << std::endl;
  }
  return buffer_[index];
}

BigInteger::base_t& BigInteger::At(size_t index) {
  if (index >= buffer_.size()) {
    std::cerr << "Out of range" << std::endl;
  }
  return buffer_[index];
}

size_t BigInteger::Size() const { return buffer_.size(); }

template <class IntegerType>
std::string BigInteger::NumberToString(IntegerType number) {
  const int ZERO_ASCII_CODE = 48;
  std::string number_string;

  while (number > 0) {
    number_string.push_back(static_cast<char>(ZERO_ASCII_CODE + number % 10));

    number /= 10;
  }

  return number_string;
}

void BigInteger::AddLeadingZeros(std::string& number_string,
                                 int segment_length) {
  const int ZERO_ASCII_CODE = 48;
  std::string temp;
  temp.assign(segment_length - number_string.size(),
              static_cast<char>(ZERO_ASCII_CODE));

  number_string = temp + number_string;
}

std::string BigInteger::toString() const {
  std::string result;
  size_t size = Size();
  const size_t SEGMENT_LENGTH = BigInteger::NumberLength(BigInteger::BASE) - 1;
  if (buffer_.size() == 1 && buffer_[0] == 0) {
    return std::string("0");
  }

  if (IsNegative()) {
    result += '-';
  }

  result += NumberToString(At(size - 1));

  for (size_t i = size - 1; i >= 1; --i) {
    std::string number_string = NumberToString(At(i - 1));
    AddLeadingZeros(number_string, SEGMENT_LENGTH);
    result += number_string;
  }

  return result;
}

 std::ostream& operator<<(std::ostream& os, const BigInteger& value) {
  os << value.toString();

  return os;
}

std::istream& operator>>(std::istream& is, BigInteger& value) {
  std::string input;
  is >> input;

  if (input.empty()) {
    value = 0;
  } else {
    value.Clear();
    value.FillBufferFromString(input);
  }

  return is;
}

void BigInteger::Clear() {
  buffer_.clear();
  is_negative_ = false;
}

bool operator<(const BigInteger& lhs, const BigInteger& rhs) {
  if (lhs.buffer_.size() == 1 && rhs.buffer_.size() == 1) {
    return lhs.buffer_[0] < rhs.buffer_[0];
  }
  if (lhs.IsNegative() == rhs.IsNegative() && !lhs.IsNegative()) {
    if (lhs.Size() < rhs.Size()) {
      return true;
    }
    if (lhs.Size() > rhs.Size()) {
      return false;
    }

    for (size_t i = lhs.Size(); i >= 1; --i) {
      if (lhs.buffer_[i - 1] < rhs.buffer_[i - 1]) {
        return true;
      }
      if (lhs.buffer_[i - 1] > rhs.buffer_[i - 1]) {
        return false;
      }
    }

    return false;
  } else if (lhs.IsNegative() == rhs.IsNegative() && lhs.IsNegative()) {
    return (-rhs) < (-lhs);
  } else {
    return lhs.IsNegative() && !rhs.IsNegative();
  }
}

bool operator>(const BigInteger& lhs, const BigInteger& rhs) {
  return rhs < lhs;
}

bool operator==(const BigInteger& lhs, const BigInteger& rhs) {
  return !(lhs < rhs || lhs > rhs);
}
bool operator!=(const BigInteger& lhs, const BigInteger& rhs) {
  return !(lhs == rhs);
}

bool operator<=(const BigInteger& lhs, const BigInteger& rhs) {
  return !(lhs > rhs);
}

bool operator>=(const BigInteger& lhs, const BigInteger& rhs) {
  return !(lhs < rhs);
}

BigInteger abs(const BigInteger& value) {
  BigInteger result(value);

  if (result.is_negative_) {
    result.is_negative_ = false;
  }

  return result;
}

BigInteger::operator bool() {
  return !(buffer_.size() == 1 && buffer_[0] == 0);
}
