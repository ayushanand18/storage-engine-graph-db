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
