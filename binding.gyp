{
    "targets": [
        {
            "target_name": "readpath",
            "sources": [ "src/dirread.cc", "src/async.cc" ],
            "include_dirs" : [
 	 			"<!(node -e \"require('nan')\")"
			]
        }
    ],
}
