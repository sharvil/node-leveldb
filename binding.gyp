{
  "targets": [
    {
      "target_name": "node_leveldb",
      "sources": [ "src/leveldb.cpp" ],
      "dependencies": [
        "<(module_root_dir)/deps/leveldb/leveldb.gyp:leveldb"
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
