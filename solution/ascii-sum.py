from pathlib import Path


def sum_ascii_values_in_file(file_path: str) -> int:
    file = Path(file_path)
    if not file.exists():
        raise FileNotFoundError(f"{file_path} not found")

    total_sum = 0
    with file.open('r') as f:
        for line in f:
            total_sum += sum(ord(char) for char in line if char != ' ' and char != '\n')

    return total_sum


sum_1 = sum_ascii_values_in_file("capy-1.txt")
sum_2 = sum_ascii_values_in_file("capy-2.txt")
product = sum_1 * sum_2

print(f"sum_1: {sum_1}")
print(f"sum_2: {sum_2}")
print(f"product: {product}")
