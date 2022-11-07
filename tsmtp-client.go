package main

import (
	"fmt"
	"log"
	"net/smtp"
)

func main() {
	// Configuration
	from := "nobody@weird.com"
	to := []string{
		"woods@planix.com",
	}
	smtpHost := "localhost"
	smtpPort := "25"

	message := []byte("Subject: testing\r\n" +
		"\r\n" +
		"Hello! I'm trying out smtp to send emails to recipients.\r\n")

	// Create authentication
	//password := "12345samplemail"
	//auth := smtp.PlainAuth("", from, password, smtpHost)

	// Send actual message
	err := smtp.SendMail(smtpHost+":"+smtpPort, nil, from, to, message)
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println("Email sent successfully")
}

//
// Local Variables:
// compile-command: (let ((fn (file-name-nondirectory (buffer-file-name)))) (concat "go vet " fn " && go build " fn))
// End:
//
