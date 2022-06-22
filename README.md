# NumberConverter

Convert integers and fixed point floats between number systems. Supports number
systems form 2 to 36. Alphabet is '0-9a-z\\.'. Input strings are case insensitive.
Leading spaces are ignored. Length of numbers is limited olny by your available
memory as the program depends on
[num_bigint::BigInt](https://docs.rs/num-bigint/0.4.3/num_bigint/index.html)
implementation of multiprecision integers (MPI).

The program is written in Rust and depends on `num_bigint` and `num-rational` for MP
integers and rationals and `clap` for CLI interface.

## Pseudocode

### Main procedure

Input:

- inputNumber - string representation of a number in system base1. inputNumber can
  contain integer part, decimal point and fraction part;
- base1 - integer, number system of input number;
- base2 - integer, number system of output number;
- precision - integer to limit length of a faction part in an output number.

Output:

- outString - string representation of inputNumber in base2. If inputNumber contains
  fractional part and precision > 0, outString can also contains one. Some numbers in
  system base1 don't have precise representation in system base2.

```
1. Split inputNumber into intPartStr - part to left of the point, and fractPartStr -
   part to the right of the point.
2. Put intPart := toInteger(intPartStr ,base1).
3. Put fractPart := Rational{toInteger(fractPartStr, base1), pow(base1,
   fractPartStr.size)} - represens fractPartStr as number from [0, 1).
4. Put outString := toString(intPart, base2).
5. If fractPart > 0 and precision > 0:
5.1. Append outString += "." + fractionPartToString(fractPart, base2, precision).
6. Return outString.
```

### Procedure toInteger

Input:

- n - string form of an MPI in system base1;
- base1 - integer.

Returns
[num_bigint::BigInt::parse_bytes](https://docs.rs/num-bigint/0.4.3/num_bigint/struct.BigInt.html#method.parse_bytes)

### Procedure pow

Input:

- a - MPI;
- n - integer.

Output:

- a raised to power n.

Returns
[num_bigint::BigInt::pow](https://docs.rs/num-bigint/0.4.3/num_bigint/struct.BigInt.html#method.pow)

### Procedure toString

Input:

- inputNumber - MPI;
- base2 - output system;

Returns
[num_bigint::BigInt::to_str_radix](https://docs.rs/num-bigint/0.4.3/num_bigint/struct.BigInt.html#method.to_str_radix)

### Procedure fractionPartToString

Input:

- num - rational number in range \[0, 1\);
- base2 - integer;
- precision - integer.

Returns `toString(floor(n * pow(base2, precision)), base2)`.

### Procedure floor

Input:

- num - rational number in range.

Returns
[num_rational::Ratio::floor](https://docs.rs/num-rational/latest/num_rational/struct.Ratio.html#method.floor)

More details in [this](src/converter.rs) file.

## Help

```
    Finished dev [unoptimized + debuginfo] target(s) in 0.04s
     Running `target/debug/number_converter --help`
number_converter
Program to convert integers and fixed point floats between number systems

USAGE:
    number_converter [OPTIONS] --from <FROM> --to <TO> <INPUT>

ARGS:
    <INPUT>    Number in system <FROM>

OPTIONS:
    -f, --from <FROM>              Input number system
    -h, --help                     Print help information
    -p, --precision <PRECISION>    Precision, digits after point in output number [default: 0]
    -t, --to <TO>                  Output number system
```

## Examples

```bash
$ cargo run -- --from 10 --to 2 255
11111111

$ cargo run -- --from 2 --to 10 11111111
255

$ cargo run -- --from 10 --to 2 --precision 20 300.1
100101100.00011001100110011001

$ cargo run -- --from 2 --to 10 --precision 20 100101100.00011001100110011001
300.09999942779541015625

$ cargo run -- --from 16 --to 2 f0f0f0
111100001111000011110000
```

# vim: textwidth=85
