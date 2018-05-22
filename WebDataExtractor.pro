include(WDEConfig.pri)

TEMPLATE = subdirs

CONFIG	+= ordered

###########################################################
## Library project
SUBDIRS += \
	WDELib \
	WDEResource \
#	utWDELib \
#	UKPostcodeHelper \
###########################################################


###########################################################
## Sample projects
SUBDIRS += \
#	MediaFireParser \
#	ContactSeeker \
#	TestContactSeeker \
#	Manta_com \
#	ncua_gov \
#	inc_com \
#	alexa_com \			# http://www.alexa.com/
#	toolstation_com \
#	exhibitor \
#	membershipcentre_rcsa_com_au \	#http://www.membershipcentre.rcsa.com.au/RCSAWebGen/Company_Listing.aspx

## Normal projects
SUBDIRS += \
#	gdorb_decc_gov_uk \ #
#	MCS-V2 \
#	ciphe_org_uk \
#	WhitePage \
#	WhitePage_AU \
#	YellowPage \
#	YellowPage_AU \
#	USGreenBuildingCouncil \
#	erkendhypotheekadviseur \
#	Century21_au \
#	Stockdaleleggo_au \
#	Paginasamarillas.es \
#	tica-acad_co_uk \
#	inca-ltd_org_uk \
#	ciga_co_uk \
#	nia_org_uk \
#	essure_com \
#	real-estate \
#	architecture_com \
#	epcregister_com \
#	homecheck_co_uk \
#	defaqto_com \
#	thekennelclub_org_uk \
	thekennelclub_org_uk2 \ # findapuppy
#	find_a_vet \
#	ic_upei_ca \
#	icatcare_org \
#	defaqto_com2 \
#	findpetboarding_com \
#	dogsandpuppies_co_uk \
#	gundogbreeders_co_uk \
#	dogbreeds_co_uk \
	breedersonline_co_uk \

###########################################################

###########################################################
## Project require MySQL database
contains(DEFINES, USE_DATABASE) {
	SUBDIRS += \
#		niceic-com \

}
###########################################################
