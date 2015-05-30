# -*- mode: ruby -*-
# vi: set ft=ruby :

# Vagrantfile API/syntax version. Don't touch unless you know what you're doing!
VAGRANTFILE_API_VERSION = "2"

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  config.vm.box = "ubuntu/trusty32"

  config.ssh.forward_agent = true
  config.ssh.forward_x11 = false

  config.vm.provider "virtualbox" do |vb|
    vb.gui = false

    vb.customize ["modifyvm", :id, "--memory", "1024"]
  end

  config.vm.provision "shell", :path => "provision.sh"
end
