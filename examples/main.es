fn fact(n: Int) -> Int {
    if n <= 1 {
        return 1;
    } else {
        return n * fact(n - 1);
    }
}

fn main() -> Void {
    let num: Int = 5;
    let fact: Int = fact(num);
}