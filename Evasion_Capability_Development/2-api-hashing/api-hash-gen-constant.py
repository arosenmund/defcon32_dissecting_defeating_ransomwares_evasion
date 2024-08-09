#Hide Capability of a Particular function

# List of API names to hash
APIsToHash = ["CreateThread","CreateRemoteThread","VirtualAlloc","IsDebuggerPresent","ShellExecuteA"]

# Iterate over each API name in the list
for api in APIsToHash:
    # Initialize the hash value with a starting value
    hash_value = 0x35
    # Initialize iteration counter
    i = 0

    # Convert each character in the API name to its integer value and process it
    for l in api:
        # Get the integer (ASCII) value of the character
        c = ord(l)
        # Convert the integer to a hexadecimal value (as integer)
        c = int(f"0x{c:x}", 16)
        # Update the hash value with a specific formula
        # The formula involves multiplying the current hash value by a constant,
        # adding the character's value, and applying a bitwise AND with 0xffffff
        hash_value += hash_value * 0xab10f29f + c & 0xffffff
        # Convert the updated hash value to a hexadecimal string for printing
        hash_hex = f"0x{hash_value:x}"
        # Increment the iteration counter
        i += 1
        # Print the current iteration details
        print(f"Iteration {i} : {l} : {c} : {hash_hex}")
    
    # Print the final hash value for the API name
    print(f"{api}\t 0x00{hash_value:x}")