# node-leveldb
This package provides a simple, lightweight binding for [LevelDB](https://github.com/google/leveldb) to [Node.js](http://www.nodejs.org). LevelDB is an implementation of a [log-structured merge-tree](http://en.wikipedia.org/wiki/Log-structured_merge-tree) that provides fast key-value storage and lookup.

## Installation
```
$ npm install node-leveldb
```

## Documentation
### LevelDB
A LevelDB class is returned when you `require('node-leveldb')`. An instance of this class can be used for all operations on a single LevelDB database.
```js
    var LevelDB = new require('node-leveldb')();
```

### LevelDB.open(databasePath)
Before operating on a LevelDB object, you have to specify which database those operations apply to. If the specified database doesn't already exist, a new one will be created. If your process doesn't have the necessary permissions to create or open the database, this function will throw an exception.
```js
    LevelDB.open('path/to/my/database');
```

### LevelDB.close()
Closes the database opened by a previous successful call to `LevelDB.open(...)`.
```js
    LevelDB.close();
```

### LevelDB.get(key)
Returns the value associated with `key`. If `key` doesn't exist in the database, this function returns `null`.
```js
    console.log(LevelDB.get('myKey'));
```

### LevelDB.set(key, value)
Inserts data into the database. If there isn't already a value for `key` in the database, this function creates a new entry and maps `key` to `value`. If `key` already exists, it replaces the old value with `value`.
```js
    LevelDB.set('Ann Leckie', 'Ancillary Justice');
```

### LevelDB.list([start], [end], callback)
Enumerates data stored in the database. This function takes two optional arguments, the `start` and `end` of the range of keys to enumerate. If `start` is not specified, it is assumed to be from the first item. If `end` is not specified, it is assumed to be until the last item. Both `start` and `end` are inclusive. It's not possible to specify `end` without first specifying `start`.
```js
    LevelDB.list('a', 'z', function(key, value) {
        console.log('key: ' + key + ', value: ' + value);
    });
```

### LevelDB.delete(key)
Removes data from the database. If `key` is found in the database, this function deletes the mapping from `key` to its value. This function returns `true` if `key` was found and a mapping was deleted, `false` otherwise.
```js
    LevelDB.delete('Ann Leckie');
```

## Example
```js
    var LevelDB = new require('node-leveldb')();
    LevelDB.open('authors_database');

    LevelDB.set('Frank Herbert', 'Dune');
    LevelDB.set('Ursula K. Le Guin', 'The Left Hand of Darkness');
    LevelDB.set('Larry Niven', 'Ringworld');
    LevelDB.set('Isaac Asimov', 'The Gods Themselves');

    // Print the value associated with 'Frank Herbert'.
    console.log(LevelDB.get('Frank Herbert'));

    LeveLDB.delete('Frank Herbert');

    // Print all items stored in the database.
    LevelDB.list(function(key, value) {
        console.log(key + ': ' + value);
    });

    // Print all items with 'F' <= key <= 'T'.
    LevelDB.list('F', 'T', function(key, value) {
        console.log(key + ': ' + value);
    });

    LevelDB.close();
```

## License
Apache 2.0
