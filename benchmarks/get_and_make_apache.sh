APACHE_VERSION=2.2.15
#PREFIX=/srv/cs523/misc/apache
PREFIX=/media/internal/apache
NUM_ATTEMPTS=1

if [ ! -d "httpd-${APACHE_VERSION}" ]; then
	wget http://www.fightrice.com/mirrors/apache/httpd/httpd-$APACHE_VERSION.tar.gz
	tar -xzf httpd-$APACHE_VERSION.tar.gz
fi

cd httpd-$APACHE_VERSION
./configure -q --prefix=$PREFIX 2>& 1 > /tmp/configure.out
for i in `seq 1 $NUM_ATTEMPTS`
do
	make --quiet clean 2>& 1 > /tmp/make_clean.out
	time make --quiet 2>&1 > /tmp/make.out
done
