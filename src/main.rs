mod converter;
mod errors;
mod parser;
mod utils;

/// Program to convert integers and fixed point floats between number systems
#[derive(clap::Parser, Debug)]
struct Args {
    /// Number in system <FROM>
    input: String,

    /// Input number system
    #[clap(short, long)]
    from: u32,

    /// Output number system
    #[clap(short, long)]
    to: u32,

    /// Precision, digits after point in output number
    #[clap(short, long, default_value = "0")]
    precision: u32,
}

fn main() {
    use clap::Parser;
    let args = Args::parse();
    match crate::converter::convert(&args.input, args.from, args.to, args.precision) {
        Ok(s) => println!("{}", s),
        Err(e) => {
            eprintln!("{}", e);
            std::process::exit(1);
        }
    }
}
