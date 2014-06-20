(require 'clojure.string)
(require 'clojure.set)

; holds the parsed ast
(def ast (read *in*))

; handles a statement by calling the function named for that statement
(defn handle-statement [datapath statement]
  (apply (-> statement first symbol resolve) datapath (rest statement)))

; maps handle-statement over a list of statements
(defn handle-statements [datapath statements]
  (map (partial handle-statement datapath) statements))

; handles the root node by handling the single function binding statement
(defn program [datapath ast]
  (handle-statement datapath ast))

; handles a statement by delegating to the specific statement's function
(defn statement [datapath statement]
  (handle-statement datapath statement))

; creates combinations of dimensions to enumerate all registers
(defn enumerate-dimensions [dims]
  (if (empty? dims)
    '(())
    (for [d (first dims)
          more (enumerate-dimensions (rest dims))]
      (cons d  more))))

; puts input or output registers into the datapath
(defn add-sized-registers [datapath registers register-type]
  (reduce (partial merge-with clojure.set/union)
          (map (fn [register]
                 (let [id (-> register rest first)
                       dims (flatten (map rest (-> register rest rest)))
                       dimensions (map range dims)
                       indices (enumerate-dimensions dimensions)
                       ids (map (partial clojure.string/join "$") indices)
                       reg-names (map (partial str id) ids)]
                   (assoc datapath register-type (set reg-names))))
               registers)))

; handles a function binding
(defn bindfun [datapath name params returns & statements]
  (merge datapath 
         (add-sized-registers datapath (rest params) :inputs)
         (add-sized-registers datapath (rest returns) :outputs)))
           
; handles a variable binding
(defn bindvar [datapath variable expression]
  )

; start parsing the program
(println (handle-statement {} ast))
