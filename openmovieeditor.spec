Name:           openmovieeditor
Version:        20060326
Release:        1%{?dist}
Summary:        simple, powerful movie editor

Group:          Applications/Multimedia
License:        GPL
URL:            http://openmovieeditor.sourceforge.net
Source0:        openmovieeditor-20060326.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  fltk-devel, libquicktime-devel, gavl-devel, libjpeg-devel, libpng-devel
#Requires:       

%description


%prep
%setup -q


%build
%configure
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT


%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
%doc AUTHORS COPYING NEWS README TODO
%{_bindir}/openmovieeditor


%changelog
