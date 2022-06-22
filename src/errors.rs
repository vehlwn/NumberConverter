#[derive(Debug, Eq, PartialEq)]
pub struct InvalidBaseError {
    pub min: u32,
    pub max: u32,
}
impl std::error::Error for InvalidBaseError {}
impl std::fmt::Display for InvalidBaseError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "Base of a number system must be from {} to {}",
            self.min, self.max
        )
    }
}

#[derive(Debug)]
pub struct ParseError;
impl std::error::Error for ParseError {}
impl std::fmt::Display for ParseError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "Invalid input format")
    }
}

#[derive(Debug)]
pub struct ValidationError {
    pub c: char,
    pub base: u32,
}
impl std::error::Error for ValidationError {}
impl std::fmt::Display for ValidationError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "Invalid digit '{}' in the base {}", self.c, self.base)
    }
}
