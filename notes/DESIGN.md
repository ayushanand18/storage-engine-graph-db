# Design Specs

## Basic Objects 
`Node`
```
{
    "data_pointer": "uuid_0x000fffe356",
    "connection_list": SortedSet([
        "node_class.node_id",
        ...
    ]),
    ...
}
```

`Data Block` Table
```
{
    "uuid_0x...": "raw_data",
    ...
}
```

`Reads Cache`
```
{
    "node_class.node_id": "Node Object<>",
    ...
}
```

## Architecture
![architecture](architecture.png)

## Flow
+ Writes
  + all writes go to an active memtable.
  + when a function value crosses threshold, dump it to old_memtables. 
    function(size, keys, ...).
    old_memtables = list[memtables]
  + flush to disc as memtables cross a theshold function
    function(size, tables, keys, ...).
  + merge SSTables, as a batch process. (to be detailed later)
  + all writes are flushed to buffer. 
    async batch buffer data to logs on disc.
