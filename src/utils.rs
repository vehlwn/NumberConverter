pub fn fraction_to_string(mut n: num_rational::BigRational, base: u32, precision: u32) -> String {
    n *= num_bigint::BigInt::from(base).pow(precision);
    let j = n.floor().numer().clone();
    let ret = j.to_str_radix(base);
    let padding: String = std::iter::repeat('0')
        .take(precision as usize - ret.len())
        .collect();
    return padding + &ret;
}
#[cfg(test)]
mod test {
    use super::*;
    #[test]
    fn test_fraction_1_100_base_10_20() {
        let n = num_rational::BigRational::new(From::from(1_u32), From::from(100_u32));
        let res = fraction_to_string(n, 10, 20);
        assert_eq!(res, "01000000000000000000");
    }
    #[test]
    fn test_fraction_1_100_base_2_20() {
        let n = num_rational::BigRational::new(From::from(1_u32), From::from(100_u32));
        let res = fraction_to_string(n, 2, 20);
        assert_eq!(res, "00000010100011110101");
    }
    #[test]
    fn test_fraction_1_16_base_10_20() {
        let n = num_rational::BigRational::new(From::from(1_u32), From::from(16_u32));
        let res = fraction_to_string(n, 10, 20);
        assert_eq!(res, "06250000000000000000");
    }
    #[test]
    fn test_fraction_1_16_base_16_20() {
        let n = num_rational::BigRational::new(From::from(1_u32), From::from(16_u32));
        let res = fraction_to_string(n, 16, 20);
        assert_eq!(res, "10000000000000000000");
    }
    #[test]
    fn test_fraction_2_16_base_10_20() {
        let n = num_rational::BigRational::new(From::from(2_u32), From::from(16_u32));
        let res = fraction_to_string(n, 10, 20);
        assert_eq!(res, "12500000000000000000");
    }
}
