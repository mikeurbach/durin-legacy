(require 'clojure.string)
(require 'clojure.set)

(defn handle-node 
  "handles a statement by calling the function named for that statement"
  [datapath statement]
  (apply (-> statement first symbol resolve) datapath (rest statement)))

(defn handle-nodes 
  "maps handle-node over a list of statements"
  [datapath statements]
  (reduce (partial merge-with clojure.set/union)
          (map (partial handle-node datapath) statements)))

(defn program 
  "handle the root node by handling the single function binding statement"
  [datapath ast]
  (handle-node datapath ast))

(defn statement 
  "handles a statement by delegating to the specific statement's function"
  [datapath statement]
  (handle-node datapath statement))

(defn sized-register 
  "destructures the form (identifer x (integer 10) (integer 10)) 
  into {:x [10 3]}"
  [reg-list]
  (let [identifier (-> reg-list rest first)
        sizes (vec (flatten (map rest (-> reg-list rest rest))))]
    (hash-map (keyword identifier) sizes)))

(defn enumerate-dimensions
  "creates combinations of dimensions to enumerate all registers"
  [dims]
  (if (empty? dims)
    '(())
    (for [d (first dims)
          more (enumerate-dimensions (rest dims))]
      (cons d  more))))

(defn add-sized-registers 
  "puts input or output registers into the datapath"
  [datapath registers register-type]
  (merge datapath
         (hash-map register-type 
                   (reduce merge {} (map sized-register registers)))))

(defn bindfun 
  "handles a function binding
   TODO: adds its keyword into the :blocks set
   adds inputs and outputs to the datapath
   handles the statements"
  [datapath name params returns & statements]
  (let [dpi (add-sized-registers datapath (rest params) :inputs)
        dpio (add-sized-registers dpi (rest returns) :outputs)]
    (handle-nodes dpio statements)))

(defn lookup-identifier
  "resolves an identifier to an input, output, register
   TODO: before here, we should have checked that identifier will be uniq
         so the order shouldn't matter
   TODO: function (after bindfun done)"
  [datapath identifier]
  (let [ident-key (keyword identifier)]
    (cond 
     (ident-key (:registers datapath)) 
       [ident-key (ident-key (:registers datapath))]
     (ident-key (:inputs datapath))
       [ident-key (ident-key (:inputs datapath))]
     (ident-key (:outputs datapath)) 
       [ident-key (ident-key (:outputs datapath))] )))

(defn lookup-operand 
  "resolves an operator to a block, identifier, or literal, returning
   [:identifier [vec]] or [literal nil]
   TODO: blocks"
  [datapath operand]
  (let [type (first operand)
        value (second operand)]
    (cond
     (= type 'identifier) (lookup-identifier datapath value)
     (or (= type 'integer) (= type 'float)) [value nil])))

(defn binary-operator 
  "handles a generic binary operator, returning
   {:unique-name {:dims [vec] :ops [op1 op2]}}"
  [datapath operator operand1 operand2]
  (let [[name1 dims1] (lookup-operand datapath operand1)
        [name2 dims2] (lookup-operand datapath operand2)
        wire-name (str operator "_" (gensym))]
    (cond
     (and dims1 dims2) 
       (if (= dims1 dims2)
         {wire-name {:dims dims1 :ops [name1 name2]}}
         (println "error: operands of different dimensions"))
     (and dims1 (nil? dims2)) {wire-name {:dims dims1 :ops [name1 name2]}}
     (and (nil? dims1) dims2) {wire-name {:dims dims2 :ops [name1 name2]}}
     (and (nil? dims1) (nil? dims2)) 
       {wire-name {:dims [1] :ops [name1 name2]}} )))

(defmacro arithmetic-operator 
  "use a macro to generate handlers for some common arithmetic operators"
  [operator] 
  `(defn ~operator [datapath# operand1# operand2#] 
     (binary-operator datapath# (keyword (quote ~operator))
                      operand1# operand2#)))

(defn make-arithmetic-operators
  "call arithmetic-operator on a list"
  []
  (doall (map (fn [op] (eval `(arithmetic-operator ~op))) 
     '[add sub mul div mod exp])))
(make-arithmetic-operators)
           
(defn bindvar 
  "handles a variable binding by recursing down the expression
   TODO: hook up the wire"
  [datapath variable expression]
  (let [wire (handle-node datapath expression)
        dpwired (assoc datapath :wires
                       (merge (:wires datapath) wire))]
    dpwired))
         
(defn generate-datapath 
  "start at the top, print the datapath
   datapath is a big hash of sets and hashes, currently: 
   inputs, outputs, registers, wires, blocks, and literals
   kick it off with a datapath consisting of known blocks"
  [ast]
  (handle-node 
   { :blocks #{:add :sub :mul :div :mod :exp} }
   ast))

; read in an ast and write out a datapath
(def ast (read-string (slurp "add1.dst")))
(println (generate-datapath ast))
