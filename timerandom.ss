(define (timerandom max)
 (let ((num 0))
   (when (< num max)
;;	  (random)
	  (set! num (1+ num)))))

(timerandom 100000)
