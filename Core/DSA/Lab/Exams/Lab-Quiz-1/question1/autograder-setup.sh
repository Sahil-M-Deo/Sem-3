#!/bin/bash

if ! command -v python3 &> /dev/null; then
    echo "ERROR: python3 not found."
    echo "Install Python 3 first."
    exit 1
fi

if ! command -v g++ &> /dev/null; then
    echo "ERROR: g++ not found."
    exit 1
fi

for required_command in /usr/bin/time /usr/bin/timeout /usr/bin/prlimit; do
    if [ ! -x "$required_command" ]; then
        echo "ERROR: $required_command not found."
        exit 1
    fi
done

if [ ! -d "venv" ]; then
    echo "Creating virtual environment..."
    python3 -m venv venv
else
    echo "Virtual environment already exists."
fi

echo "Activating virtual environment..."
source venv/bin/activate

echo "Upgrading pip..."
pip install --upgrade pip

echo "Installing required packages..."
pip install -r requirements.txt

python grader.py all
