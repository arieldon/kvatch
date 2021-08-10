# Kvatch
Kvatch is an in-memory key-value store.


## Usage
Kvatch aims to provide a simple REST API. Once Kvatch is running, three
HTTP methods are available: GET, PUT, and DELETE.

Add a new value to the table:
```
$ curl -vX PUT -d "some value" localhost:4000/some_key
```

Retrieve a value from the table:
```
$ curl -v localhost:4000/some_key
```

Update an existing value in the table:
```
$ curl -vX PUT -d "new value" localhost:4000/some_key
```

Delete a value from the table:
```
$ curl -vX DELETE localhost:4000/some_key
```

Additional examples are available in `scripts/test.py`.


## Installation
Kvatch does not depend on libraries outside of standard C or POSIX.
```
$ ./scripts/build.sh
# ./scripts/build.sh --install
```


## Sources
This project steals liberally from a few sources -- some prose, some
code, most both.

- [Drew DeVault's gmnisrv](https://git.sr.ht/~sircmpwn/gmnisrv)
- [Ben Hoyt on Hash Tables](https://benhoyt.com/writings/hash-table-in-c/)
- [Bob Nystrom of Crafting Interpreters on Hash Tables](http://www.craftinginterpreters.com/hash-tables.html)
- [Austin Z. Henley on Challenging Projects](https://web.eecs.utk.edu/~azh/blog/morechallengingprojects.html)
