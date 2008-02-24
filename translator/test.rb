def intro
  puts "Hi there. Here's an example demonstrating RubyEx's functionality."
  puts "We're demonstrating block functionality here, as well as some simple method calls on numbers."
  puts "More than that, we're going to be defining some functions. Exciting! :)"
end

def counter(n, m)
  1.upto(n) do |x|
    p x
    x.upto(m) {|y| p y}
  end
end

intro; counter 5, 10
