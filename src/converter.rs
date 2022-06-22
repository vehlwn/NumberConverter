const MIN_BASE: u32 = 2;
const MAX_BASE: u32 = 36;

lazy_static::lazy_static! {
    static ref VALID_DIGITS:Vec<char> = ('0'..='9').chain('a'..='z').collect();
}

fn validate(
    parse_result: &crate::parser::ParseResult,
    base: u32,
) -> Result<(), crate::errors::ValidationError> {
    let is_valid_char = |c: char| {
        VALID_DIGITS
            .iter()
            .take(base as usize)
            .find(|&&x| x == c.to_ascii_lowercase())
            .is_some()
    };
    let find_invalid = |s: &str| s.chars().find(|&c| !is_valid_char(c));
    let validate_string = |s| {
        return match find_invalid(s) {
            Some(c) => Err(crate::errors::ValidationError { c, base }),
            None => Ok(()),
        };
    };
    let ret = validate_string(parse_result.int_part);
    return match parse_result.fract_part {
        Some(x) => ret.and(validate_string(x)),
        None => ret,
    };
}

pub fn convert(
    input: &str,
    base1: u32,
    base2: u32,
    precision: u32,
) -> Result<String, Box<dyn std::error::Error>> {
    if (base1 < MIN_BASE || MAX_BASE < base1) || (base2 < MIN_BASE || MAX_BASE < base2) {
        return Err(Box::new(crate::errors::InvalidBaseError {
            min: MIN_BASE,
            max: MAX_BASE,
        }));
    }
    let parse_result = crate::parser::parse_int_fract(input)?;
    validate(&parse_result, base1)?;
    let int_part =
        num_bigint::BigInt::parse_bytes(parse_result.int_part.as_bytes(), base1).unwrap();
    let mut ret = int_part.to_str_radix(base2);
    if parse_result.fract_part.is_some() && precision > 0 {
        let fract_part_str = parse_result.fract_part.unwrap();
        let fract_part = num_rational::BigRational::new(
            num_bigint::BigInt::parse_bytes(fract_part_str.as_bytes(), base1).unwrap(),
            num_bigint::BigInt::from(base1).pow(fract_part_str.len() as u32),
        );
        ret.push('.');
        let s = crate::utils::fraction_to_string(fract_part, base2, precision);
        ret.push_str(&s);
    }
    return Ok(ret);
}

#[cfg(test)]
mod test {
    use super::*;
    #[test]
    fn test_validate_full_valid() {
        let input = "  123azc.456abczjkl";
        let res = crate::parser::parse_int_fract(input).unwrap();
        super::validate(&res, super::MAX_BASE).unwrap();
    }
    #[test]
    fn test_invalid_int_part() {
        let input = "  123456789";
        let res = crate::parser::parse_int_fract(input).unwrap();
        let res = super::validate(&res, 9);
        assert!(res.is_err());
        let err = res.err().unwrap();
        assert_eq!(err.c, '9');
        assert_eq!(err.base, 9);
    }
    #[test]
    fn test_invalid_fract_part() {
        let input = "  0.123456789";
        let res = crate::parser::parse_int_fract(input).unwrap();
        let res = super::validate(&res, 9);
        assert!(res.is_err());
        let err = res.err().unwrap();
        assert_eq!(err.c, '9');
        assert_eq!(err.base, 9);
    }
    #[test]
    fn test_convert_invalid_base() {
        assert!(convert("", 1, 1, 0).is_err());
        assert!(convert("", 1, 37, 0).is_err());
        assert!(convert("", 37, 1, 0).is_err());
        assert!(convert("", 37, 37, 0).is_err());
    }
    #[test]
    fn test_convert_255_base_10_10_0() {
        let input = "255";
        let res = convert(input, 10, 10, 0);
        assert!(res.is_ok());
        let res = res.unwrap();
        assert_eq!(res, input);
    }
    #[test]
    fn test_convert_255_base_10_2_0() {
        let input = "255";
        let res = convert(input, 10, 2, 0);
        assert!(res.is_ok());
        let res = res.unwrap();
        assert_eq!(res, "11111111");
    }
    #[test]
    fn test_convert_255_5_base_10_2_0() {
        let input = "255.5";
        let res = convert(input, 10, 2, 1);
        assert!(res.is_ok());
        let res = res.unwrap();
        assert_eq!(res, "11111111.1");
    }
    #[test]
    fn test_convert_255_625_base_10_2_0() {
        let input = "255.625";
        let res = convert(input, 10, 2, 10);
        assert!(res.is_ok());
        let res = res.unwrap();
        assert_eq!(res, "11111111.1010000000");
    }
}
