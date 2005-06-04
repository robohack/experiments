#! awk -f

#
#  expects smail logs on input, finds and prints message size
#

$5 == "Received" {
        received++;
        program = "";
        protocol = "";
        for (fn = 6; fn <= NF; fn++) {
                if (substr($fn, 1, 5) == "SIZE:") {
                        size = substr($fn, 6) + 0;
			break;		
                }
        }
	print size;
}
