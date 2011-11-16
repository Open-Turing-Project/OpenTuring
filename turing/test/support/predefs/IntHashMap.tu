unit
module pervasive IntHashMap
    export New,Free,Put,Get,Remove
    
    external "hashmap_new" fcn New () : int
    external "hashmap_free" proc Free (id : int)
    
    external "hashmap_put" proc Put (id : int,key:string,val:int)
    external "hashmap_get" fcn Get (id : int,key:string,var res : int) : int
    external "hashmap_remove" proc Remove (id : int,key:string)
end IntHashMap
