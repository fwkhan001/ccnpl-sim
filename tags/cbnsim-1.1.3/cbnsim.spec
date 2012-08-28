#
# $Id: cbnsim.spec,v 1.5 2006/02/24 13:10:16 carzanig Exp $
#
%define name	cbnsim
%define ver	%version
%define rel	1

Summary:   Content-Based Networking Simulation Library
Name:      %{name}
Version:   %{ver}
Release:   %rel
License:   GPL
Group:     Applications/Internet
Source:    http://www.cs.colorado.edu/serl/cbn/%{name}-%{ver}.tar.gz
URL:       http://www.cs.colorado.edu/serl/cbn/
Packager:  Matthew J. Rutherford <rutherfo@cs.colorado.edu>

BuildRoot: %{_topdir}/rpmroot

%description 

Content-Based Networking Simulation Library

%prep
rm -rf $RPM_BUILD_ROOT

%setup 

%build

CFLAGS="$RPM_OPT_FLAGS" ./configure $RPMBUILD_CONFIGURE_FLAGS --prefix=/usr --disable-dependency-tracking
make all

%install
#
# install_doc=no prevents make install from installing the html
# documentation.  We do this here because that is taken care of by the
# %doc script below
#
make DESTDIR="$RPM_BUILD_ROOT" install_doc=no install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,755)
%doc AUTHORS COPYING INSTALL NEWS README ChangeLog
#
# doc is the documentation directory, and html is the value of
# HTML_OUTPUT in *both* doc/Makefile.am and doc/doxygen.conf.in
# 
%doc doc/html
%attr(755,root,root) /usr/bin/*
%attr(644,root,root) /usr/include/cbnsim/*
%attr(755,root,root) /usr/lib/lib*.so*
%attr(644,root,root) /usr/lib/*.*a
%attr(644,root,root) /usr/share/%{name}-%{ver}/*

%post
ldconfig

%postun
ldconfig

