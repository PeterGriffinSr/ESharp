fn main() -> Void {
    let value: String = "a";
    switch (value) {
        case "a" -> print("Nice Letter");
        case "b" -> print("Ew");
        default -> print("Not an option");
    }
}