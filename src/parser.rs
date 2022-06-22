pub struct ParseResult<'a> {
    pub int_part: &'a str,
    pub fract_part: Option<&'a str>,
}

pub fn parse_int_fract<'a>(input: &'a str) -> Result<ParseResult<'a>, crate::errors::ParseError> {
    let int_part_re = r"(?P<int_part>[0-9a-z]+)";
    let fract_part_re = r"(?P<fract_part>[0-9a-z]+)";
    let re =
        regex::Regex::new(&format!(r"^\s*{0}([.]{1}?)?$", int_part_re, fract_part_re)).unwrap();
    let caps = match re.captures(input) {
        None => {
            return Err(crate::errors::ParseError);
        }
        Some(x) => x,
    };
    let int_part = caps.name("int_part").unwrap().as_str();
    let fract_part = caps.name("fract_part").map(|x| x.as_str());
    return Ok(ParseResult {
        int_part,
        fract_part,
    });
}

#[cfg(test)]
mod test {
    #[test]
    fn parse_all() {
        let input = "  123azc.456abczjkl";
        let res = crate::parser::parse_int_fract(input).unwrap();
        assert_eq!(res.int_part, "123azc");
        assert_eq!(res.fract_part, Some("456abczjkl"));
    }
    #[test]
    fn parse_int_part() {
        let input = "  123azc";
        let res = crate::parser::parse_int_fract(input).unwrap();
        assert_eq!(res.int_part, "123azc");
        assert_eq!(res.fract_part, None);
    }
    #[test]
    fn parse_int_part_with_dot() {
        let input = "  123azc.";
        let res = crate::parser::parse_int_fract(input).unwrap();
        assert_eq!(res.int_part, "123azc");
        assert_eq!(res.fract_part, None);
    }
    #[test]
    fn parse_fract_part() {
        let input = "  0.123azc";
        let res = crate::parser::parse_int_fract(input).unwrap();
        assert_eq!(res.int_part, "0");
        assert_eq!(res.fract_part, Some("123azc"));
    }
}
