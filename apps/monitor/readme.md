# How to use

To install this dev project run the following commands in an elevated command prompt. Make sure your current directory is set to this present directory:

Intall node dependencies
```
$ npm install
```
Install build tools. This will install python 2.7 amongst others. If you have a mode recent version of python, you will have to set your default python to 2.7
```
$ npm install --global --production windows-build-tools
```

Rebuild node dependencies
```
$ ./node_modules/.bin/electron-rebuild
```
Run the project :)
```
npm start
```