Name:           libtasks
Version:        1.3
Release:        1%{?dist}
Summary:        A simple task system written in C++11 that implements the leader/follower pattern and uses libev.

Group:          System Environment/Libraries
License:        GNU GPL
URL:            https://github.com/apohl79/libtasks
Source0:        libtasks.tar.bz2       

BuildRequires:  cmake,devtoolset-2-toolchain,boost-devel,thrift,thrift-cpp-devel,libev-devel,cppunit-devel,git
Requires:       thrift,boost,libev4

%description


%package        devel
Summary:        Development files for %{name}
Group:          Development/Libraries
Requires:       %{name} = %{version}-%{release}

%description    devel
The %{name}-devel package contains libraries and header files for
developing applications that use %{name}.


%prep
rm -rf %{name}
git clone https://github.com/apohl79/libtasks.git %{name}

%build
rm -rf ${RPM_BUILD_ROOT}
cmake %{_builddir}/%{name} -DCMAKE_INSTALL_PREFIX:PATH=${RPM_BUILD_ROOT}/usr
make

%install
rm -rf $RPM_BUILD_ROOT
make install
cd ${RPM_BUILD_ROOT}/usr/lib64
ln -s %{name}.so %{name}.so.%{version} 

%clean
rm -rf $RPM_BUILD_ROOT


%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%defattr(-,root,root,-)
%doc
%{_libdir}/*.so
%{_libdir}/*.so.*

%files devel
%defattr(-,root,root,-)
%doc
%{_includedir}/*
#%{_libdir}/*.so


%changelog
