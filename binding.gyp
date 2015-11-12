{
    "targets": [
        {
            "target_name": "dirread",
            "sources": [ "src/dirread.cc", "src/async.cc" ],
            "include_dirs" : [
 	 			"<!(node -e \"require('nan')\")"
			]
        }
    ],
}
