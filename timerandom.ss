(define (timerandom max)
  (let ((num 0))
    (when (< num max)
      (random 128000)
      (set! num (add1 num)))))

(timerandom 10000000)
