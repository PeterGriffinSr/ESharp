fn fact(n: int) -> int {
    if n <= 1 {
        return 1;
    } else {
        return n * fact(n - 1);
    }
}

fn main() -> void {
    let num: int = 5;
    let fact: int = fact(num);~
}