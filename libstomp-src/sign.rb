require 'openssl'
require 'base64'
data = File.read('data.txt')
digest = OpenSSL::Digest::SHA1.new
pkey = OpenSSL::PKey::RSA.new(File.read('/home/pdebois/john-private.pem'))
signature = pkey.sign(digest, data)
puts Base64.encode64(signature)
