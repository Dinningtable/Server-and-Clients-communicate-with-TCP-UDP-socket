import os
from subprocess import PIPE, Popen
import subprocess
import difflib
from concurrent.futures import ThreadPoolExecutor
import argparse
import concurrent.futures
import socket
import pathlib
from time import sleep

SLEEP_FOR_SOCKET_READY = 0.025

parser = argparse.ArgumentParser(
    'Sample demo script\n   To ensure your zip format is correct, please make sure to run this script before submission\n'
)

parser.add_argument('-p', dest='port', type=int, default=8888)
parser.add_argument('-t', '--testcase', dest='testcase', type=str, default='')
args = parser.parse_args()

BUILD_DIR = 'build'
SERVER_YOUR = f'{BUILD_DIR}/server'
CLIENT_YOUR = f'{BUILD_DIR}/client'
SERVER_PORT = args.port

TESTCASES_SRC = "testcases"
RESPONSE = "response"
TESTCASES_CORRECT = 'testcases_correct'
TESTCASES_DIFF = "testcases_diff"

os.makedirs(BUILD_DIR, exist_ok=True)
os.makedirs(RESPONSE, exist_ok=True)
os.makedirs(TESTCASES_DIFF, exist_ok=True)
testcases_src = [args.testcase] if args.testcase else os.listdir(TESTCASES_SRC)


def run_testcase(filename):
    print(f'Running {filename}')
    try:
        proc = Popen([SERVER_YOUR, f'{SERVER_PORT}'])
    except FileNotFoundError as e:
        print(
            f'{e}\n   Please make sure your server is in {BUILD_DIR}, U might forget to run build.sh'
        )
        exit(1)
    sleep(SLEEP_FOR_SOCKET_READY)

    # To ensure server has started listening
    while True:
        p = subprocess.run(['lsof', '-i', f':{SERVER_PORT}'],
                           stdout=PIPE,
                           stderr=PIPE)
        if p.stdout:
            break

    with open(os.path.join(TESTCASES_SRC ,filename), 'r') as input_f:
        with open(os.path.join(RESPONSE ,filename), 'w+') as out_f:
            subprocess.run([CLIENT_YOUR, "127.0.0.1", f'{SERVER_PORT}'], stdout= out_f, stdin=input_f)
    proc.terminate()


def diff_file(filename):
    ta_answer = None
    your_answer = None

    src_file = os.path.join(TESTCASES_CORRECT, filename)
    with open(src_file, 'r') as f:
        ta_answer = f.readlines()

    dst_file = os.path.join(RESPONSE, filename)
    with open(dst_file, 'r') as f:
        your_answer = f.readlines()

    assert ta_answer and your_answer
    dif = difflib.context_diff(ta_answer,
                               your_answer,
                               fromfile=src_file,
                               tofile=dst_file)
    dif_list = list(dif)
    if not dif_list:
        return 1

    diff_file = os.path.join(TESTCASES_DIFF, filename)
    with open(diff_file, 'w+') as f:
        f.writelines(iter(dif_list))
    return 0


for filename in testcases_src:
    run_testcase(filename)
    print(f'{filename} finished')

with ThreadPoolExecutor() as executor:
    future_to_filename = {
        executor.submit(diff_file, filename): filename
        for filename in testcases_src
    }
    total_pass_testcases = 0
    for future in concurrent.futures.as_completed(future_to_filename):
        filename = future_to_filename[future]
        try:
            score = future.result()
            if score == 0:
                print(f'testcases/{filename} is not correct')
            else:
                total_pass_testcases += score

        except Exception as e:
            print(e)

    print(f'PASS {total_pass_testcases}/{len(testcases_src)}')
