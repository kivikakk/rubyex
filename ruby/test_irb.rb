#!./ruby

input = :continue
line_count = 0
chaining = ""
chaining_mode = false
_ = nil

while input == :continue
  line_count += 1
  print "rxi(main):#{line_count}:0#{if chaining_mode; "*"; else ">"; end} "
  input = gets
  chaining_mode = false

  if input.nil?
    puts
  else
    input.strip!
    if input.length > 0
      begin
	puts "=> " + (_ = eval(chaining + input + "\n")).inspect
	chaining = ""
      rescue Exception => e
	if e.class == SyntaxError
	  if e.message.index "unexpected $end"
	    # This is so not robust.
	    chaining_mode = true
	    chaining += "#{input}\n"
	  else
	    chaining = ""; puts "#{e.class}: #{e.message}"
	  end
	else
	  chaining = ""; puts "#{e.class}: #{e.message}"
	end
      end
    end
    input = :continue
  end
end

