#!/usr/bin/env node
let ret: { [key: string]: string } = {};
for (let i: number = 0; i < 5000000; i++) {
    ret["id." + (i % 500000)] = "val." + i;
}
console.log(Object.keys(ret).length);
console.log(ret["id.10000"]);
