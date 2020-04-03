;;
;; This works well with SBCL (pkgsrc/lang/sbcl); and together with Sly installed
;; in Emacs, you can open this file and eval expressions with C-x C-e
;;
;;
;; From:
;;
;; https://dataswamp.org/~solene/2020-02-04-awk-like-commonlisp.html
;;
(defmacro awk(file separator &body code)
  "allow running code for each line of a text file,
   giving access to NF and NR variables, and also to
   fields list containing fields, and line containing $0"
    `(progn
       (let ((stream (open ,file :if-does-not-exist nil)))
         (when stream
           (loop for line = (read-line stream nil)
              counting t into NR
              while line do
                (let* ((fields (uiop:split-string line :separator ,separator))
                       (NF (length fields)))
                  ,@code))))))

;; equivalent of awk '{ print NF }' file | sort | uniq
;; for counting how many differents fields long line we have
;;
;; xxx missing the predicate for `sort' and there is no `uniq'
;;
;(uniq (sort (awk "tawk.sbcl" " " NF) ???))

;; numbering lines of a text file with NR
;; awk '{ print NR": "$0 }' file.txt
;;
(awk "tawk.sbcl" " "
     (format t "~a: ~a~%" NR line))

;; printing 4th field
;; awk -F ';' '{ print $4 }' data.csv
;;
(awk "/etc/passwd" ":"
     (print (nth 4 fields)))

;; display NF-1 field (yes it's -2 in the example because -1 is last field in the list)
;; awk -F ';' '{ print NF-1 }' file.csv
;;
(awk "/etc/passwd" ":"
     (print (nth (- NF 2) fields)))

;; filtering lines (like grep)
;; awk '/unbound/ { print }' /var/log/messages
;;
(awk "/var/log/messages" " "
     (when (search "core" line)
       (print line)))
