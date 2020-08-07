#! ./berry
import os

os.system('lcov', '-c -i -d . -o init.info')

var exec = './berry'
var path = 'tests'
var testcases = os.listdir(path)
var total = 0, failed = 0

for i : testcases
    if os.path.splitext(i)[1] == '.be'
        print('\033[0;36mrun testcase: ' + i + '\033[0m')
        var ret = os.system(exec, os.path.join(path, i))
        if ret != 0
            print('\033[0;31mreturn code:', ret, '\033[0m')
            failed += 1
        end
        total += 1
    end
end

print('\033[0;32mtest results: ' +
    str(total) + ' total, ' + str(failed) + ' failed' +
    (failed ? '' : ' (all tests passed)') +
    '.\033[0m')

os.system('lcov', '-c -d ./ -o cover.info')
os.system('lcov', '--remove total.info */usr/include/* -o final.info')
os.system('genhtml', '-o test_report --legend --title "lcov" --prefix=./ final.info')
os.system('rm', '-f', 'init.info cover.info final.info')

os.exit(failed == 0)
