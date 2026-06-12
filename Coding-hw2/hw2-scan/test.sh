echo "Running tests..."
echo

output=$(./test)
expected_output="Correct"

if [ $? -eq 0 ] ; then
  echo "Pass program check"
else
  echo "Fail: Program did not exit zero"
  exit 1
fi

if [ "$output" == "$expected_output" ] ; then
  echo "Pass: Scan output is correct"
else
  echo "Fail: Scan ouput is not correct"
  exit 1
fi

echo
echo "All tests passed."

exit 0