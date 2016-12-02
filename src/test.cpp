//
//  test.cpp
//
//  by jiahuan.liu
//  10/12/2016
//

#include <queue>

#include "Socket.h"
#include "Http.h"
#include "Crawler.h"

void test_chat_client(const std::string& sAddr, const uint16_t uPort);
void test_chat_server(const uint16_t uPort);
void test_url();
int  test_request(std::string sUrl, HttpResponse& stRsp, int iTimeout = 3000);
void test_crawler();
void test_lock();
void test_rwlock();

#include "SearchServer.h"

void test_indexer();
void test_searcher();
void test_json();
void test_json_index();
void test_searchImpl();

int main()
{
    LOG << "begin" << END;
    __TRY__
    //test_searchImpl();
    //test_json();
    test_searcher();
    //test_chat_server(2222);
    __CATCH__(Exception& e)
    LOG << e.what() << END;
    __END__
    LOG << "end" << END;
}

const int BUF_SIZE = 1024;
void test_chat_client(const std::string& sAddr, const uint16_t uPort)
{
    Socket sock;
    sock.create(SOCK_STREAM, AF_INET);
    sock.connect(sAddr, uPort);
    char msg[1024] = "\0";
    while (true)
    {
        std::cin >> msg;
        if (strcmp(msg, "exit") == 0)
            break;
        if (sock.send(msg, (int)strlen(msg), 0) < 0)
            throw Exception("Socket::send: error:", errno);
        if (sock.recv(msg, BUF_SIZE, 0) <= 0)
            throw Exception("Socket::recv: error:", errno);
        LOG << "[" << sAddr << ":" << uPort << "] " << msg << END;
    }
}

void test_chat_server(const uint16_t uPort)
{
    Socket sock, client;
    sockaddr stClientAddr; socklen_t iLen;
    sock.create(SOCK_STREAM, AF_INET);
    sock.bind(uPort);
    sock.listen(1);
    char msg[1024] = "\0";
    int count = 0;
    while (strcmp(msg, "close"))
    {
        client = Socket();
        sock.accept(client, &stClientAddr, iLen);
        LOG << "#" << count << " client accepted." << END;
        count++;
        while (true)
        {
            memset(msg, 0, BUF_SIZE);
            if (client.recv(msg, BUF_SIZE, 0) <= 0)
            {
                LOG << "client disconnected." << END;
                break;
            }
            else
            {
                if (strcmp(msg, "close") == 0)
                    break;
                LOG << "[recv]: " << msg << END;
                /*if (client.send(msg, (int)strlen(msg), 0) < 0)
                {
                    LOG << "client disconnected." << END;
                    break;
                }*/
            }
        }
    }
}

void test_url()
{
    Url url;
    url.parse("http://www.qq.com:8080/abc/dev/query?a=b&c=3#ref");
    LOG << url.getRef() << END
    << url.getQuery() << END
    << url.getHost() << END
    << url.getPort() << END
    << url.getResourcePath() << END
    << url.getRootPath() << END
    << url.getRelatPath() << END
    << url.genUrl() << END;
}

int test_request(std::string sUrl, HttpResponse& stRsp, int iTimeout)
{
    HttpRequest req;
    req.createGetRequest(sUrl, true);
    HttpHeader* header = req.getHeader();
    req.setUserAgent("Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.8.1.14)");
    header->setAccept("text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5");
    header->setAcceptLanguage("en");
    header->setItem("Accept-Charset", "utf-8;q=0.7,*;q=0.7");
    
    std::string sSend = req.encode();
    LOG << sSend << END;
    
    Url url = req.getUrl();
    TcpClient tcp;
    tcp.init(url.getHost(), Common::strto<uint16_t>(url.getPort()), iTimeout);
    
    int iRet = tcp.send(sSend.c_str(), sSend.length());
    if (iRet != ClientSocket::EM_SUCCESS)
    {
        return iRet;
    }
    
    stRsp.reset();
    std::string sRecv;
    size_t iRecv = 0, iBuf = 10240, iCur;
    char* sBuf = new char[iBuf];
    
    while (true)
    {
        iCur = iBuf;
        iRet = tcp.recv(sBuf, iCur);
        if (iRet == ClientSocket::EM_CLOSE)
        {
            break;
        }
        else if (iRet != ClientSocket::EM_SUCCESS)
        {
            return iRet;
        }
        iRecv += iCur;
        sRecv.append(sBuf, iCur);
    }
    
    iRet = stRsp.decode(sRecv.c_str(), sRecv.length());
    LOG << stRsp.encode() << END;
    
    delete []sBuf;
    
    return iRet == true ? 0 : -9;
}

void test_crawler()
{
    std::string sHostUrl = "http://www.reuters.com/";
    int iArticleTrdNum = 16;
    int iArchiveTrdNum = 2;
    int iListTrdNum = 4;
    int iMaxDoc = 1000000;
    std::string sWorkPlace = "./";
    std::set<std::string> targetArchive = {"/news/archive/worldNews",
                                            "/news/archive/businessNews",
                                            "/news/archive/technologyNews"};
    MyCrawler crawler;
    crawler.init(sHostUrl, iArticleTrdNum, iArchiveTrdNum, iListTrdNum, iMaxDoc, sWorkPlace, targetArchive);
    crawler.run();
}

#include "ThreadSync.h"
#include "ThreadPool.h"

void test_lock()
{
    ThreadPool pool(4, 4, false);
    
    ThreadLock lock;
    
    auto th0 = [&]
    {
        int loop = 3;
        while (loop--)
        {
            //std::this_thread::sleep_for(std::chrono::seconds(1));
            std::lock_guard<ThreadLock> l(lock);
            printf("notify()\n");
            //std::lock_guard<ThreadLock> l(lock);
            //lock.notify();
        }
    };
    auto th1 = [&]
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        //lock.wait();
        std::lock_guard<ThreadLock> l(lock);
        printf("th1 exits\n");
    };
    auto th2 = [&]
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        //lock.wait();
        std::lock_guard<ThreadLock> l(lock);
        printf("th2 exits\n");
    };
    auto th3 = [&]
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        //lock.wait();
        std::lock_guard<ThreadLock> l(lock);
        printf("th3 exits\n");
    };
    
    pool.addTask(th0);
    pool.addTask(th1);
    pool.addTask(th2);
    pool.addTask(th3);
    
    pool.start();
    pool.stop();
    pool.join();
}

#include <sstream>
#include <iostream>

class AtomicWrite : public std::ostream
{
public:
    AtomicWrite();
    ~AtomicWrite();
private:
    std::ostringstream _os;
};

AtomicWrite::AtomicWrite()
: std::ostream(0)
, _os()
{
    this->init(_os.rdbuf());
}

AtomicWrite::~AtomicWrite()
{
    _os << '\n';
    std::cout << _os.str();
}

void test_rwlock()
{
    ThreadPool pool(3, 3, false);
    
    RWLock rwlock;
    
    FILE* file = fopen("test_log_file", "w");
    
    auto th0 = [&]
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        WLock wlock(rwlock);
        printf("begin writing\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        printf("end writing\n");
    };
    auto th1 = [&]
    {
        int loop = 10;
        while (loop--)
        {
            RLock rlock(rwlock);
            //char str[] = "th1 reading. th1 reading. th1 reading.\n";
            //std::cout << "th1 reading. th1 reading. th1 reading." << std::endl;
            //AtomicWrite() << "th1 reading. th1 reading. th1 reading." << std::endl;
            //fwrite(str, 1, strlen(str), stdout);
            FLOG_INFO(file) << "th1 reading. th1 reading. th1 reading.";
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        printf("th1 exits\n");
    };
    auto th2 = [&]
    {
        int loop = 10;
        while (loop--)
        {
            RLock rlock(rwlock);
            //char str[] = "th2 reading. th2 reading. th2 reading.\n";
            //std::cout << "th2 reading. th2 reading. th2 reading." << std::endl;
            //AtomicWrite() << "th2 reading. th2 reading. th2 reading." << std::endl;
            //fwrite(str, 1, strlen(str), stdout);
            FLOG_INFO(file) << "th2 reading. th2 reading. th2 reading.";
            //sem.notify();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        printf("th2 exits\n");
    };
    
    pool.addTask(th0);
    pool.addTask(th1);
    pool.addTask(th2);
    
    pool.start();
    pool.stop();
    pool.join();
    fclose(file);
}

void test_indexer()
{
    /*search::InvertedIndex index1;
    Indexer::loadHadoop(index1, "test_hadoop_index");
    Indexer::save(index1, "test_save_index");
    search::InvertedIndex index2;
    Indexer::load(index2, "test_save_index");
    assert(index1 == index2);*/
}

void test_searcher()
{
    Searcher searcher(4, 128);
    searcher.load("lucene_json_index", "docs.txt");
    searcher.start();
    std::vector<result_t> res;
    searcher.doSearchAsyn({"china"}, 10, [](std::vector<result_t> res){std::cout << search::toJson(res) << std::endl;});
    searcher.doSearchAsyn({"china", "economy"}, 10, [](std::vector<result_t> res){std::cout << search::toJson(res) << std::endl;});
    searcher.stop();
}

void test_json()
{
    vres_t list =
    {
        {"title1", "url1"},
        {"title2", "url2"},
        {"title3", "url3"},
        {"title4", "url4"},
        {"title5", "url5"},
        {"title6", "url6"},
        {"title7", "url7"},
        {"title8", "url8"},
        {"title9", "url9"},
        {"title10", "url10"}
    };
    std::string json = search::toJson(list);
    std::cout << json << std::endl;
}

void test_json_index()
{
    search::InvertedIndex index;
    LOG_INFO << "begin loading";
    Indexer::loadJson(index, "lucene_json_index");
    LOG_INFO << "end loading";
    assert(index["1.19"].vhit.size() == 12);
    assert(index["braggadocious"].vhit.size() == 3);
    assert(index["salla"].vhit[0].docid == 10872);
    assert(index["salla"].vhit[0].freq == 2);
}

void test_searchImpl()
{
    SearchImpl impl;
    LOG_INFO << "begin loading";
    impl.load("lucene_json_index", "docs.txt");
    LOG_INFO << "end loading";
    std::vector<result_t> result;
    
    LOG_INFO << "begin searching";
    impl.doSearch({"china"}, 10, result);
    LOG_INFO << "end searching";
    result.clear();
    LOG_INFO << "begin searching";
    impl.doSearch({"china", "economy"}, 10, result);
    LOG_INFO << "end searching";
}

//================================================
//HTTP request header example：
//Host：rss.sina.com.cn
//User-Agent：Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.8.1.14) Gecko/20080404 Firefox/2.0.0.14
//Accept：text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5
//Accept-Language：zh-cn,zh;q=0.5
//Accept-Encoding：gzip,deflate
//Accept-Charset：gb2312,utf-8;q=0.7,*;q=0.7
//Keep-Alive：300
//Connection：keep-alive
//Cookie：userId=C5bYpXrimdmsiQmsBPnE1Vn8ZQmdWSm3WRlEB3vRwTnRtW
//If-Modified-Since：Sun, 01 Jun 2008 12:05:30 GMT
//Cache-Control：max-age=0
//
//
//HTTP response header example：
//Status：OK - 200
//Date：Sun, 01 Jun 2008 12:35:47 GMT
//Server：Apache/2.0.61 (Unix)
//Last-Modified：Sun, 01 Jun 2008 12:35:30 GMT
//Accept-Ranges：bytes
//Content-Length：18616
//Cache-Control：max-age=120
//Expires：Sun, 01 Jun 2008 12:37:47 GMT
//Content-Type：application/xml
//Age：2
//X-Cache：HIT from 236-41.D07071951.sina.com.cn
//Via：1.0 236-41.D07071951.sina.com.cn:80 (squid/2.6.STABLE13)
//Connection：close
//================================================