/*Copyright (c) 2015 Max Ogden and other contributors
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.*/


/*
    The following code is a slightly modified version of the default electron-
    packager command-line interface (cli.js)
    https://github.com/electron-userland/electron-packager
 */

const electronRebuild = require('electron-rebuild');
const common = require('./node_modules/electron-packager/common')
const fs = require('fs')
const packager = require('electron-packager')
const usage = fs.readFileSync('./node_modules/electron-packager/usage.txt').toString()
const copyfiles = require('copyfiles')

var args = common.parseCLIArgs(process.argv.slice(2))

// temporary fix for https://github.com/nodejs/node/issues/6456
var stdioWriters = [process.stdout, process.stderr]
stdioWriters.forEach(function (stdioWriter) {
    if (stdioWriter._handle && stdioWriter._handle.setBlocking) {
        stdioWriter._handle.setBlocking(true)
    }
})

function printUsageAndExit(isError) {
    var print = isError ? console.error : console.log
    print(usage)
    process.exit(isError ? 1 : 0)
}

if (args.help) {
    printUsageAndExit(false)
} else if (args.version) {
    if (typeof args.version !== 'boolean') {
        console.error('--version does not take an argument. Perhaps you meant --app-version or --electron-version?\n')
    }
    console.log(common.hostInfo())
    process.exit(0)
} else if (!args.dir) {
    printUsageAndExit(true)
}

// Native module rebuild function
function nativePackageRebuild(buildPath, electronVersion, platform, arch, callback) {
    console.log('Rebuilding native modules');
    electronRebuild.rebuild({ buildPath, electronVersion, arch })
        .then(() => callback())
        .catch((error) => callback(error));
}

// Required files must be copied
function includeFiles(appDirectory) {
    console.log('Copying required files');

    copyfiles(['./gmaps-api.txt','./bee/*.json','./pages/*', './' + appDirectory], {}, (err) => {
        if (err) {
            console.error('Failed to copy files:')
            console.error(err)
        }
    })
}

// Adding after copy functions
if (args.afterCopy)
    args.afterCopy.push(nativePackageRebuild);
else
    args.afterCopy = [nativePackageRebuild];

packager(args)
    .then(function done(appPaths) {
        if (appPaths.length > 1) console.error('Wrote new apps to:\n' + appPaths.join('\n'))
        else if (appPaths.length === 1) console.error('Wrote new app to', appPaths[0])

        // Copy files to apps
        appPaths.forEach((path) => {
            includeFiles(path);
        })

        return true
    }).catch(function error(err) {
        if (err.message) console.error(err.message)
        else console.error(err, err.stack)
        process.exit(1)
    })
