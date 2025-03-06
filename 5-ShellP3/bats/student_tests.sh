#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Command with Two Pipes" {
    run "./dsh" <<EOF
echo "hello world" | grep "hello" | wc -c
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="12dsh3>dsh3>cmdloopreturned0"
    
    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Command with Pipes and Built-In Commands" {
    run "./dsh" <<EOF
echo "hello, world" | grep "hello" | exit
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="dsh3>Exiting..."
    
    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Empty Pipe" {
    run "./dsh" <<EOF
|
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="dsh3>warning:nocommandsprovideddsh3>cmdloopreturned0"
    
    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Too Many Commands Exceed Pipe Limit" {
    many_commands=$(printf '%s' "echo 1 | echo 2 | echo 3 | echo 4 | echo 5 | echo 6 | echo 7 | echo 8 | echo 9")
    run ./dsh <<EOF                 
${many_commands}
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="dsh3>error:pipinglimitedto8commandsdsh3>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Executable Exceeds Max Size" {
    long_exe=$(printf '%0.sx' {1..65})
    run ./dsh <<EOF                   
${long_exe}
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="dsh3>error:maxbuffersizeforshellis320dsh3>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Argument Exceeds Max Size" {
    long_arg=$(printf '%0.sx' {1..257})
    run ./dsh <<EOF                  
echosd "${long_arg}"
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="dsh3>error:maxbuffersizeforshellis320dsh3>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Piping With Nonexistent Command" {
    run "./dsh" <<EOF
i_dont_exist | cat
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="execvp:Nosuchfileordirectorydsh3>dsh3>dsh3>cmdloopreturned0"
    
    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}