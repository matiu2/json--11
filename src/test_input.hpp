#pragma once

#include <string>

const std::string realJson =
    R"XXX({"access":{"token":{"id":"abcdxxxxxxxxxxxxxxxxxxxxxxxxxxxx",")XXX"
    R"XXX(expires":"2014-07-05T12:08:39.527Z","tenant":{"id":"12345X",")XXX"
    R"XXX(name":"12345X"},"RAX-AUTH:authenticatedBy":["APIKEY"]},")XXX"
    R"XXX(serviceCatalog":[{"name":"cloudBlockStorage","endpoints":[{")XXX"
    R"XXX(region":"SYD","tenantId":"12345X","publicURL":"https:\/\/syd.)XXX"
    R"XXX(blockstorage.api.rackspacecloud.com\/v1\/12345X"},{"region":"DFW")XXX"
    R"XXX(,"tenantId":"12345X","publicURL":"https:\/\/dfw.blockstorage.api.)XXX"
    R"XXX(rackspacecloud.com\/v1\/12345X"},{"region":"ORD","tenantId":")XXX"
    R"XXX(12345X","publicURL":"https:\/\/ord.blockstorage.api.)XXX"
    R"XXX(rackspacecloud.com\/v1\/12345X"},{"region":"IAD","tenantId":")XXX"
    R"XXX(12345X","publicURL":"https:\/\/iad.blockstorage.api.)XXX"
    R"XXX(rackspacecloud.com\/v1\/12345X"},{"region":"HKG","tenantId":")XXX"
    R"XXX(12345X","publicURL":"https:\/\/hkg.blockstorage.api.)XXX"
    R"XXX(rackspacecloud.com\/v1\/12345X"}],"type":"volume"},{"name":")XXX"
    R"XXX(cloudImages","endpoints":[{"region":"IAD","tenantId":"12345X",")XXX"
    R"XXX(publicURL":"https:\/\/iad.images.api.rackspacecloud.)XXX"
    R"XXX(com\/v2\/12345X"},{"region":"ORD","tenantId":"12345X","publicURL")XXX"
    R"XXX(:"https:\/\/ord.images.api.rackspacecloud.com\/v2\/12345X"},{")XXX"
    R"XXX(region":"HKG","tenantId":"12345X","publicURL":"https:\/\/hkg.)XXX"
    R"XXX(images.api.rackspacecloud.com\/v2"},{"region":"DFW","tenantId":")XXX"
    R"XXX(12345X","publicURL":"https:\/\/dfw.images.api.rackspacecloud.)XXX"
    R"XXX(com\/v2\/12345X"},{"region":"SYD","tenantId":"12345X","publicURL")XXX"
    R"XXX(:"https:\/\/syd.images.api.rackspacecloud.com\/v2\/12345X"}],")XXX"
    R"XXX(type":"image"},{"name":"cloudQueues","endpoints":[{"region":"HKG")XXX"
    R"XXX(,"tenantId":"12345X","publicURL":"https:\/\/hkg.queues.api.)XXX"
    R"XXX(rackspacecloud.com\/v1\/12345X","internalURL":"https:\/\/snet-)XXX"
    R"XXX(hkg.queues.api.rackspacecloud.com\/v1\/12345X"},{"region":"ORD",")XXX"
    R"XXX(tenantId":"12345X","publicURL":"https:\/\/ord.queues.api.)XXX"
    R"XXX(rackspacecloud.com\/v1\/12345X","internalURL":"https:\/\/snet-)XXX"
    R"XXX(ord.queues.api.rackspacecloud.com\/v1\/12345X"},{"region":"SYD",")XXX"
    R"XXX(tenantId":"12345X","publicURL":"https:\/\/syd.queues.api.)XXX"
    R"XXX(rackspacecloud.com\/v1\/12345X","internalURL":"https:\/\/snet-)XXX"
    R"XXX(syd.queues.api.rackspacecloud.com\/v1\/12345X"},{"region":"DFW",")XXX"
    R"XXX(tenantId":"12345X","publicURL":"https:\/\/dfw.queues.api.)XXX"
    R"XXX(rackspacecloud.com\/v1\/12345X","internalURL":"https:\/\/snet-)XXX"
    R"XXX(dfw.queues.api.rackspacecloud.com\/v1\/12345X"},{"region":"IAD",")XXX"
    R"XXX(tenantId":"12345X","publicURL":"https:\/\/iad.queues.api.)XXX"
    R"XXX(rackspacecloud.com\/v1\/12345X","internalURL":"https:\/\/snet-)XXX"
    R"XXX(iad.queues.api.rackspacecloud.com\/v1\/12345X"}],"type":"rax:)XXX"
    R"XXX(queues"},{"name":"cloudOrchestration","endpoints":[{"region":")XXX"
    R"XXX(HKG","tenantId":"12345X","publicURL":"https:\/\/hkg.)XXX"
    R"XXX(orchestration.api.rackspacecloud.com\/v1\/12345X"},{"region":")XXX"
    R"XXX(DFW","tenantId":"12345X","publicURL":"https:\/\/dfw.)XXX"
    R"XXX(orchestration.api.rackspacecloud.com\/v1\/12345X"},{"region":")XXX"
    R"XXX(ORD","tenantId":"12345X","publicURL":"https:\/\/ord.)XXX"
    R"XXX(orchestration.api.rackspacecloud.com\/v1\/12345X"},{"region":")XXX"
    R"XXX(IAD","tenantId":"12345X","publicURL":"https:\/\/iad.)XXX"
    R"XXX(orchestration.api.rackspacecloud.com\/v1\/12345X"},{"region":")XXX"
    R"XXX(SYD","tenantId":"12345X","publicURL":"https:\/\/syd.)XXX"
    R"XXX(orchestration.api.rackspacecloud.com\/v1\/12345X"}],"type":")XXX"
    R"XXX(orchestration"},{"name":"cloudServersOpenStack","endpoints":[{")XXX"
    R"XXX(region":"SYD","tenantId":"12345X","publicURL":"https:\/\/syd.)XXX"
    R"XXX(servers.api.rackspacecloud.com\/v2\/12345X","versionInfo":"https:)XXX"
    R"XXX(\/\/syd.servers.api.rackspacecloud.com\/v2","versionList":"https:)XXX"
    R"XXX(\/\/syd.servers.api.rackspacecloud.com\/","versionId":"2"},{")XXX"
    R"XXX(region":"DFW","tenantId":"12345X","publicURL":"https:\/\/dfw.)XXX"
    R"XXX(servers.api.rackspacecloud.com\/v2\/12345X","versionInfo":"https:)XXX"
    R"XXX(\/\/dfw.servers.api.rackspacecloud.com\/v2","versionList":"https:)XXX"
    R"XXX(\/\/dfw.servers.api.rackspacecloud.com\/","versionId":"2"},{")XXX"
    R"XXX(region":"IAD","tenantId":"12345X","publicURL":"https:\/\/iad.)XXX"
    R"XXX(servers.api.rackspacecloud.com\/v2\/12345X","versionInfo":"https:)XXX"
    R"XXX(\/\/iad.servers.api.rackspacecloud.com\/v2","versionList":"https:)XXX"
    R"XXX(\/\/iad.servers.api.rackspacecloud.com\/","versionId":"2"},{")XXX"
    R"XXX(region":"HKG","tenantId":"12345X","publicURL":"https:\/\/hkg.)XXX"
    R"XXX(servers.api.rackspacecloud.com\/v2\/12345X","versionInfo":"https:)XXX"
    R"XXX(\/\/hkg.servers.api.rackspacecloud.com\/v2","versionList":"https:)XXX"
    R"XXX(\/\/hkg.servers.api.rackspacecloud.com\/","versionId":"2"},{")XXX"
    R"XXX(region":"ORD","tenantId":"12345X","publicURL":"https:\/\/ord.)XXX"
    R"XXX(servers.api.rackspacecloud.com\/v2\/12345X","versionInfo":"https:)XXX"
    R"XXX(\/\/ord.servers.api.rackspacecloud.com\/v2","versionList":"https:)XXX"
    R"XXX(\/\/ord.servers.api.rackspacecloud.com\/","versionId":"2"}],")XXX"
    R"XXX(type":"compute"},{"name":"autoscale","endpoints":[{"region":"ORD")XXX"
    R"XXX(,"tenantId":"12345X","publicURL":"https:\/\/ord.autoscale.api.)XXX"
    R"XXX(rackspacecloud.com\/v1.0\/12345X"},{"region":"DFW","tenantId":")XXX"
    R"XXX(12345X","publicURL":"https:\/\/dfw.autoscale.api.rackspacecloud.)XXX"
    R"XXX(com\/v1.0\/12345X"},{"region":"HKG","tenantId":"12345X",")XXX"
    R"XXX(publicURL":"https:\/\/hkg.autoscale.api.rackspacecloud.com\/v1.)XXX"
    R"XXX(0\/12345X"},{"region":"IAD","tenantId":"12345X","publicURL":")XXX"
    R"XXX(https:\/\/iad.autoscale.api.rackspacecloud.com\/v1.0\/12345X"},{")XXX"
    R"XXX(region":"SYD","tenantId":"12345X","publicURL":"https:\/\/syd.)XXX"
    R"XXX(autoscale.api.rackspacecloud.com\/v1.0\/12345X"}],"type":"rax:)XXX"
    R"XXX(autoscale"},{"name":"cloudDatabases","endpoints":[{"region":"SYD")XXX"
    R"XXX(,"tenantId":"12345X","publicURL":"https:\/\/syd.databases.api.)XXX"
    R"XXX(rackspacecloud.com\/v1.0\/12345X"},{"region":"DFW","tenantId":")XXX"
    R"XXX(12345X","publicURL":"https:\/\/dfw.databases.api.rackspacecloud.)XXX"
    R"XXX(com\/v1.0\/12345X"},{"region":"ORD","tenantId":"12345X",")XXX"
    R"XXX(publicURL":"https:\/\/ord.databases.api.rackspacecloud.com\/v1.)XXX"
    R"XXX(0\/12345X"},{"region":"IAD","tenantId":"12345X","publicURL":")XXX"
    R"XXX(https:\/\/iad.databases.api.rackspacecloud.com\/v1.0\/12345X"},{")XXX"
    R"XXX(region":"HKG","tenantId":"12345X","publicURL":"https:\/\/hkg.)XXX"
    R"XXX(databases.api.rackspacecloud.com\/v1.0\/12345X"}],"type":"rax:)XXX"
    R"XXX(database"},{"name":"cloudBigData","endpoints":[{"region":"DFW",")XXX"
    R"XXX(tenantId":"12345X","publicURL":"https:\/\/dfw.bigdata.api.)XXX"
    R"XXX(rackspacecloud.com\/v1.0\/12345X"},{"region":"ORD","tenantId":")XXX"
    R"XXX(12345X","publicURL":"https:\/\/ord.bigdata.api.rackspacecloud.)XXX"
    R"XXX(com\/v1.0\/12345X"}],"type":"rax:bigdata"},{"name":"cloudBackup",)XXX"
    R"XXX("endpoints":[{"region":"IAD","tenantId":"12345X","publicURL":")XXX"
    R"XXX(https:\/\/iad.backup.api.rackspacecloud.com\/v1.0\/12345X"},{")XXX"
    R"XXX(region":"HKG","tenantId":"12345X","publicURL":"https:\/\/hkg.)XXX"
    R"XXX(backup.api.rackspacecloud.com\/v1.0\/12345X"},{"region":"SYD",")XXX"
    R"XXX(tenantId":"12345X","publicURL":"https:\/\/syd.backup.api.)XXX"
    R"XXX(rackspacecloud.com\/v1.0\/12345X"},{"region":"DFW","tenantId":")XXX"
    R"XXX(12345X","publicURL":"https:\/\/dfw.backup.api.rackspacecloud.)XXX"
    R"XXX(com\/v1.0\/12345X"},{"region":"ORD","tenantId":"12345X",")XXX"
    R"XXX(publicURL":"https:\/\/ord.backup.api.rackspacecloud.com\/v1.)XXX"
    R"XXX(0\/12345X"}],"type":"rax:backup"},{"name":"cloudLoadBalancers",")XXX"
    R"XXX(endpoints":[{"region":"SYD","tenantId":"12345X","publicURL":")XXX"
    R"XXX(https:\/\/syd.loadbalancers.api.rackspacecloud.com\/v1.0\/12345X")XXX"
    R"XXX(},{"region":"IAD","tenantId":"12345X","publicURL":"https:\/\/iad.)XXX"
    R"XXX(loadbalancers.api.rackspacecloud.com\/v1.0\/12345X"},{"region":")XXX"
    R"XXX(ORD","tenantId":"12345X","publicURL":"https:\/\/ord.)XXX"
    R"XXX(loadbalancers.api.rackspacecloud.com\/v1.0\/12345X"},{"region":")XXX"
    R"XXX(HKG","tenantId":"12345X","publicURL":"https:\/\/hkg.)XXX"
    R"XXX(loadbalancers.api.rackspacecloud.com\/v1.0\/12345X"},{"region":")XXX"
    R"XXX(DFW","tenantId":"12345X","publicURL":"https:\/\/dfw.)XXX"
    R"XXX(loadbalancers.api.rackspacecloud.com\/v1.0\/12345X"}],"type":")XXX"
    R"XXX(rax:load-balancer"},{"name":"cloudMonitoring","endpoints":[{")XXX"
    R"XXX(tenantId":"12345X","publicURL":"https:\/\/monitoring.api.)XXX"
    R"XXX(rackspacecloud.com\/v1.0\/12345X"}],"type":"rax:monitor"},{"name")XXX"
    R"XXX(:"cloudDNS","endpoints":[{"tenantId":"12345X","publicURL":"https:)XXX"
    R"XXX(\/\/dns.api.rackspacecloud.com\/v1.0\/12345X"}],"type":"rax:dns"})XXX"
    R"XXX(,{"name":"cloudServers","endpoints":[{"tenantId":"12345X",")XXX"
    R"XXX(publicURL":"https:\/\/servers.api.rackspacecloud.com\/v1.)XXX"
    R"XXX(0\/12345X","versionInfo":"https:\/\/servers.api.rackspacecloud.)XXX"
    R"XXX(com\/v1.0","versionList":"https:\/\/servers.api.rackspacecloud.)XXX"
    R"XXX(com\/","versionId":"1.0"}],"type":"compute"},{"name":")XXX"
    R"XXX(cloudFilesCDN","endpoints":[{"region":"ORD","tenantId":")XXX"
    R"XXX(MossoCloudFS_b3ce8b22-f8af-4540-8d88-7fb3c604d238","publicURL":")XXX"
    R"XXX(https:\/\/cdn2.clouddrive.com\/v1\/MossoCloudFS_b3ce8b22-f8af-)XXX"
    R"XXX(4540-8d88-7fb3c604d238"},{"region":"SYD","tenantId":")XXX"
    R"XXX(MossoCloudFS_b3ce8b22-f8af-4540-8d88-7fb3c604d238","publicURL":")XXX"
    R"XXX(https:\/\/cdn4.clouddrive.com\/v1\/MossoCloudFS_b3ce8b22-f8af-)XXX"
    R"XXX(4540-8d88-7fb3c604d238"},{"region":"DFW","tenantId":")XXX"
    R"XXX(MossoCloudFS_b3ce8b22-f8af-4540-8d88-7fb3c604d238","publicURL":")XXX"
    R"XXX(https:\/\/cdn1.clouddrive.com\/v1\/MossoCloudFS_b3ce8b22-f8af-)XXX"
    R"XXX(4540-8d88-7fb3c604d238"},{"region":"HKG","tenantId":")XXX"
    R"XXX(MossoCloudFS_b3ce8b22-f8af-4540-8d88-7fb3c604d238","publicURL":")XXX"
    R"XXX(https:\/\/cdn6.clouddrive.com\/v1\/MossoCloudFS_b3ce8b22-f8af-)XXX"
    R"XXX(4540-8d88-7fb3c604d238"},{"region":"IAD","tenantId":")XXX"
    R"XXX(MossoCloudFS_b3ce8b22-f8af-4540-8d88-7fb3c604d238","publicURL":")XXX"
    R"XXX(https:\/\/cdn5.clouddrive.com\/v1\/MossoCloudFS_b3ce8b22-f8af-)XXX"
    R"XXX(4540-8d88-7fb3c604d238"}],"type":"rax:object-cdn"},{"name":")XXX"
    R"XXX(cloudFiles","endpoints":[{"region":"ORD","tenantId":")XXX"
    R"XXX(MossoCloudFS_b3ce8b22-f8af-4540-8d88-7fb3c604d238","publicURL":")XXX"
    R"XXX(https:\/\/storage101.ord1.clouddrive.com\/v1\/MossoCloudFS_)XXX"
    R"XXX(b3ce8b22-f8af-4540-8d88-7fb3c604d238","internalURL":"https:)XXX"
    R"XXX(\/\/snet-storage101.ord1.clouddrive.com\/v1\/MossoCloudFS_)XXX"
    R"XXX(b3ce8b22-f8af-4540-8d88-7fb3c604d238"},{"region":"SYD","tenantId")XXX"
    R"XXX(:"MossoCloudFS_b3ce8b22-f8af-4540-8d88-7fb3c604d238","publicURL":)XXX"
    R"XXX("https:\/\/storage101.syd2.clouddrive.com\/v1\/MossoCloudFS_)XXX"
    R"XXX(b3ce8b22-f8af-4540-8d88-7fb3c604d238","internalURL":"https:)XXX"
    R"XXX(\/\/snet-storage101.syd2.clouddrive.com\/v1\/MossoCloudFS_)XXX"
    R"XXX(b3ce8b22-f8af-4540-8d88-7fb3c604d238"},{"region":"DFW","tenantId")XXX"
    R"XXX(:"MossoCloudFS_b3ce8b22-f8af-4540-8d88-7fb3c604d238","publicURL":)XXX"
    R"XXX("https:\/\/storage101.dfw1.clouddrive.com\/v1\/MossoCloudFS_)XXX"
    R"XXX(b3ce8b22-f8af-4540-8d88-7fb3c604d238","internalURL":"https:)XXX"
    R"XXX(\/\/snet-storage101.dfw1.clouddrive.com\/v1\/MossoCloudFS_)XXX"
    R"XXX(b3ce8b22-f8af-4540-8d88-7fb3c604d238"},{"region":"IAD","tenantId")XXX"
    R"XXX(:"MossoCloudFS_b3ce8b22-f8af-4540-8d88-7fb3c604d238","publicURL":)XXX"
    R"XXX("https:\/\/storage101.iad3.clouddrive.com\/v1\/MossoCloudFS_)XXX"
    R"XXX(b3ce8b22-f8af-4540-8d88-7fb3c604d238","internalURL":"https:)XXX"
    R"XXX(\/\/snet-storage101.iad3.clouddrive.com\/v1\/MossoCloudFS_)XXX"
    R"XXX(b3ce8b22-f8af-4540-8d88-7fb3c604d238"},{"region":"HKG","tenantId")XXX"
    R"XXX(:"MossoCloudFS_b3ce8b22-f8af-4540-8d88-7fb3c604d238","publicURL":)XXX"
    R"XXX("https:\/\/storage101.hkg1.clouddrive.com\/v1\/MossoCloudFS_)XXX"
    R"XXX(b3ce8b22-f8af-4540-8d88-7fb3c604d238","internalURL":"https:)XXX"
    R"XXX(\/\/snet-storage101.hkg1.clouddrive.com\/v1\/MossoCloudFS_)XXX"
    R"XXX(b3ce8b22-f8af-4540-8d88-7fb3c604d238"}],"type":"object-store"}],")XXX"
    R"XXX(user":{"id":"158405","roles":[{"tenantId":"MossoCloudFS_b3ce8b22-)XXX"
    R"XXX(f8af-4540-8d88-7fb3c604d238","id":"5","description":"A Role that )XXX"
    R"XXX(allows a user access to keystone Service )XXX"
    R"XXX(methods","name":"object-store:default"},{"tenantId":"12345X","id")XXX"
    R"XXX(:"6","description":"A Role that allows a user access to keystone )XXX"
    R"XXX(Service )XXX"
    R"XXX(methods","name":"compute:default"},{"id":"3","description":"User )XXX"
    R"XXX(Admin )XXX"
    R"XXX(Role.","name":"identity:user-admin"},{"id":"10000150",")XXX"
    R"XXX(description":"Checkmate Access )XXX"
    R"XXX(role","name":"checkmate"}],"name":"mister_awesome","RAX-AUTH:)XXX"
    R"XXX(defaultRegion":"ORD"}}})XXX";

