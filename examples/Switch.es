fn main() -> void {
    let value: str = input("a or b");
    switch (value) {
        case "a", "c" -> {
            printf("%s\n", "Nice letter");
        }
        case "b" -> {
            printf("%s\n", "Ew");
        }
        default -> {
            printf("%s", "Not an option");
        }
    }
}
